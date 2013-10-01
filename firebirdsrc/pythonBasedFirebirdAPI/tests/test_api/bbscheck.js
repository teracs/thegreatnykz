

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
				ok(!data.message.error, "right user right password");
			} 
		});
		return 5;
}

test("bbscheck",function(){
		apiurl = (typeof host["bbscheck"] == "string" ? host["bbscheck"] : typeof host == "string" ? host+"/bbscheck" : "bbscheck"  );
		console.log(apiurl);
		expect(checkPasswd());
});