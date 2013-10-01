;
; BIND data file for local loopback interface
;
$TTL	3600
example.com.	IN	SOA	example.com. root.exapmle.com. (
			      1		; Serial
			 604800		; Refresh
			  86400		; Retry
			2419200		; Expire
			 604800 )	; Negative Cache TTL
;
	IN	NS ns
ns	IN	A	1.2.3.4

sub  IN NS	ns.sub
ns.sub IN A 1.2.3.5

