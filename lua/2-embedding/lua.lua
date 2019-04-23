print("Printing with printf:")

for n = 1, 10 do
	local buffer=n.." sec\n"
	sleep(1000);
	string_print(buffer)
	io.write( string_reverse(buffer) )
end


