-module(mc_tcpsrv).

-include("mc_tcpsrv.hrl").

-behaviour(gen_server).

-export([start_link/2]).

-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	terminate/2, code_change/3]).

-record(state, {lsock, srvpid, childnum, acceptor}).

-export([child_accept/1]).

-export([split_cmdline/3, parse_cmdline/1, recv_cmdline/2, recv_data/3]).

start_link(Ipaddr,Port)->
	gen_server:start_link( {local,list_to_atom("mc_tcpsrv_"++Ipaddr++"_"++integer_to_list(Port))}, ?MODULE,
		#tsconf{ipaddr=Ipaddr, port=Port}, []).

start_child(State = #state{lsock=_Lsock, srvpid=_Spid, childnum=Num}) ->
	ok, Cpid = spawn_link(?MODULE, child_accept, [State]),
	{ok, State#state{childnum=Num+1, acceptor=Cpid}}.

init( _Tsconf = #tsconf{ipaddr=Ipaddr, port=Port})  ->
	case inet:getaddr(Ipaddr, inet) of
		{ok, Addr} -> 
			case gen_tcp:listen(Port,[binary, {ip,Addr}, {packet, raw}, {active, false}] ) of
				{ok, ListenSock} -> 
					State = #state{lsock=ListenSock, srvpid=self(), childnum=0},
					process_flag(trap_exit, true),
					{ok, State1} = start_child(State),
					{ok, State1};
				{error, R} ->
					{error, R}
			end;
		{error, R}->
			{error, R}
	end.

child_started(Spid) ->
	gen_server:cast(Spid, c_started).

handle_cast(c_started, State) ->
	{ok, State1} = start_child(State),
	{noreply, State1}.

handle_call(A, _From, State) ->
	{reply, A, State}.

handle_info( {'EXIT', _Pid, _R}, #state{childnum=Num}=State) ->
	{noreply, State#state{childnum=Num-1}}.

terminate(shutdown, _S) ->
	ok.

code_change(_A, S, _B) ->
	{ok, S}.




-record( cstate, {sock, st=recv_cmd, cmd, args, data, datasize, buf}).

child_accept( #state{lsock=Lsock, srvpid=Spid}) ->
	case gen_tcp:accept(Lsock) of
		{ok, Sock} -> 
			child_started(Spid),
			child_srv(#cstate{sock=Sock, buf= <<>> });
		{error, R} ->
			child_started(Spid),
			io:format("child_loop(~w) failed: ~w~n", [self(),R])
	end.

child_srv(#cstate{sock=Sock,st=recv_cmd, buf=Buf }=Cst) ->
	{Cmd, Args, Datasize, Buf1} = recv_cmdline(Sock, Buf),
	child_srv( Cst#cstate{ st=recv_data, cmd=Cmd, args=Args, datasize=Datasize, buf=Buf1});

child_srv(#cstate{sock=Sock, st=recv_data, datasize=Datasize, buf=Buf }=Cst) ->
	{Data, Buf1} = recv_data(Sock, Datasize, Buf),
	child_srv( Cst#cstate{ st=serv_client, data=Data, buf=Buf1 } );

child_srv(#cstate{sock=Sock, st=serv_client, cmd=Cmd, args=Args, data=Data} = Cst) ->
	ok.

tcp_recv(Sock, Buf, Size) ->
	case gen_tcp:recv(Sock, Size) of
		{ok, Bin} ->
			<<Buf,Bin>>;
		{error, R} ->
			throw({tcp_error, R})
	end.

recv_data(_Sock, 0, Buf) ->
	{none, Buf};
recv_data(Sock, Datasize, Buf) when size(Buf) < 2+Datasize ->
	case tcp_recv(Sock, Buf, 2+Datasize-size(Buf)) of
		<<Data:Datasize/binary, $\r, $\n>> -> 
			{Data, <<>>};
		_Buf1 ->
			throw( {cmd_syntax_error, "not enough data received"} )
	end;
recv_data(_Sock, Datasize, Buf) ->
	case Buf of 
		<<Data:Datasize/binary, $\r, $\n, RestBuf/binary>> ->
			{Data, RestBuf};
		_Buf1 ->
			throw( {cmd_syntax_error, "invalid data format"} )
	end.


recv_cmdline(Sock, Buf) when size(Buf)<7 ->
	recv_cmdline(Sock, Buf, "", 0 );

recv_cmdline(Sock, Buf) ->
	try_split_cmdline( Sock, Buf, "", 0).

recv_cmdline(Sock, Buf, Acc, Size) ->
	Buf1 = tcp_recv(Sock, Buf, 0),
	try_split_cmdline(Sock, Buf1, Acc, Size).

try_split_cmdline(Sock, Buf, Acc, Size) ->
	case split_cmdline( Buf, Acc, Size ) of
		{ok, RestBuf, Cmdline} ->
			{Cmd, {Args, Datasize}} = parse_cmdline(Cmdline),
			{Cmd, Args, Datasize, RestBuf};
		{need_more, RestBuf, Acc1, Size1} ->
			recv_cmdline(Sock, RestBuf, Acc1, Size1)
	end.

split_cmdline(_Bin, _Acc, Size) when Size > 1024 ->
	throw({cmd_syntax_error, "no crln found"});

split_cmdline(<<$\r, $\n, Bin1/binary>>, Acc, _Size) ->
	{ok, Bin1, lists:reverse(Acc)};

split_cmdline(<<$\r>>=Bin, Acc, Size)  ->
	{need_more, Bin, Acc, Size };

split_cmdline(<<>>=Bin, Acc, Size) ->
	{nedd_more, Bin, Acc, Size};

split_cmdline(<<C,Bin1/binary>>, Acc, Size) ->
	split_cmdline(Bin1, [C|Acc], 1+Size).

parse_cmdline(Line) when length(Line)>0 ->
	[Cmd|Args] = string:tokens(Line, " \t"),
	validate_cmd(Cmd, Args);

parse_cmdline(_Line) ->
	throw({cmd_syntax_error, "zero length command line"}).

validate_cmd(_Cmd, Args) when length(Args) =:= 0 ->
	throw({cmd_syntax_error, "no argument"});
validate_cmd("set", Args) ->
	{set, validate_common_args(Args)};
validate_cmd("add", Args) ->
	{add, validate_common_args(Args)};
validate_cmd("append", Args) ->
	{append, validate_common_args(Args)};
validate_cmd("prepend", Args) ->
	{prepend, validate_common_args(Args)};
validate_cmd("replace", Args) ->
	{replace, validate_common_args(Args)};
validate_cmd("cas", Args) ->
	{cas, validate_cas_args(Args)};
validate_cmd("get", Args) ->
	{get, validate_get_args(Args)};
validate_cmd("gets", Args) ->
	{get, validate_get_args(Args)};
validate_cmd(Cmd, _Args) ->
	throw({cmd_syntax_error, "unknown command "++Cmd}).

throw_args(Args) ->
	throw({cmd_syntax_error, "unknown arguments: "++string:join(Args," ")}).

validate_common_args([Key, SFlags, SExptime, SBytes]=Args) ->
	try begin 
		Flags = list_to_integer(SFlags),
		Exptime = list_to_integer(SExptime),
		Bytes = list_to_integer(SBytes),
		case ( (Flags > -1) and (Exptime > -1) and (Bytes > -1)) of
			true ->
				{{Key, Flags, Exptime, Bytes}, Bytes};
			false ->
				throw_args(Args)
		end
	end
	catch
		error:_R -> throw_args(Args)
	end;

validate_common_args(Args) ->
	throw_args(Args).

validate_cas_args([Key, SFlags, SExptime, SBytes, SCas]=Args) ->
	try begin	
		{{Key, Flags, Exptime, Bytes}, DataSize} = validate_common_args([Key, SFlags, SExptime, SBytes]),
		{{Key, Flags, Exptime, Bytes, list_to_integer(SCas)}, DataSize}
	end
	catch
		error:_R -> throw_args(Args)
	end;

validate_cas_args(Args) ->
	throw_args(Args).
	
validate_get_args(Args) when length(Args) > 0 ->
	{Args,0}.



