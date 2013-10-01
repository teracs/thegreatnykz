

function checkPasswd(){
	

		url = apiurl + "?user=FireBirdAPI&pw=1234";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) {
				equal(typeof data.message.error,"string","ERROR EXIST:right user wrong password");
				equal(data.message.no,1001,"NO 1001: right user wrong password");
			} 
		});

		url = apiurl + "?user=nouser&pw=1234";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) { 
				equal(typeof data.message.error,"string","ERROR EXIST:right user wrong password");
				equal(data.message.no,1001,"NO 1001:right user wrong password");
			} 
		});

		url = apiurl + "?user=FirebirdAPI&pw=Firebird";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) { 
				notEqual(data.message.no,1001,"Not NO 1001:right user right password");
			} 
		});
		return 5;
}

function bbsuser(){
		url = apiurl + "?user=FirebirdAPI&pw=Firebird";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) {
				equal(typeof data.message.error,"string","no id");
				equal(data.message.no,2001,"NO 2001: no id");
			} 
		});
		
		url = apiurl + "?user=FirebirdAPI&pw=Firebird&id=noid";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) {
				equal(typeof data.message.error,"string","wrong id");
				equal(data.message.no,1002,"NO 1002: wrong id");
			} 
		});
		
		url = apiurl + "?user=FirebirdAPI&pw=Firebird&id=scaret";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) {
				equal(data.message.userid,"scaret","username: scaret");
			} 
		});
		return 5;
}

test("bbsuser",function(){
		apiurl = (typeof host["bbsuser"] == "string" ? host["bbsuser"] : typeof host == "string" ? host+"/bbsuser" : "bbsuser"  );
		console.log(apiurl);
		expect(checkPasswd() +bbsuser());
});