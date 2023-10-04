local i = 0

print("In file worker thread")

while true do --i < 10000000 do
	i = (i + 10000002)%10000001 -- +1 slowly
end

print("End of file worker thread")

return 'File Done'