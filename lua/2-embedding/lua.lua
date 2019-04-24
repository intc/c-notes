print([[
This is a LUA program running.

We call some C functions:

* string_print()
* sleep()
* string_reverse()

Let's go!
]])

for n = 1, 5 do
	local buffer="Saippukauppias says: "..n.."€ for "..n.." kilograms of soap.\n"
	--local buffer=n.." sec\n"
	sleep(1000);
	string_print(buffer)
	print("And let's see the same reversed:")
	io.write( string_reverse(buffer).."\n" )
end


