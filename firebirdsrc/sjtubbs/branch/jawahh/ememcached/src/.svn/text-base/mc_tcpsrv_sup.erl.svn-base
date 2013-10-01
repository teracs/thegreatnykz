-module(mc_tcpsrv_sup).

-behaviour(supervisor).

-export([start_link/0]).

-export([init/1]).


start_link() ->
	supervisor:start_link({local,?MODULE}, ?MODULE, [["0.0.0.0", 4444],["0.0.0.0", 5444]]).

init(Args) ->
	{ok, { {one_for_one, 1, 20},
		[ child_spec(L) || L <- Args ]
		}}.

child_spec([E1,E2|[]]=L) ->
	{ list_to_atom("mc_tcpsrv_"++E1++"_"++integer_to_list(E2)),
		{mc_tcpsrv, start_link, L}, permanent, 10, worker, [mc_tcpsrv] }.
