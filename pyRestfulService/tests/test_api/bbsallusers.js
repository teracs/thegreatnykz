

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

function bbsallusers(){
		url = apiurl + "?user=FirebirdAPI&pw=Firebird";
		console.log(url);

		$.ajax({ 
			async: false, 
			type : "GET", 
			url : url, 
			dataType : 'json', 
			success : function(data) {
				equal(typeof data.message.length,"number","all users")
			} 
		});
		return 1;
}

test("bbsallusers",function(){
		apiurl = (typeof host["bbsallusers"] == "string" ? host["bbsallusers"] : typeof host == "string" ? host+"/bbsallusers" : "bbsallusers"  );
		console.log(apiurl);
		expect(checkPasswd() +bbsallusers());
});