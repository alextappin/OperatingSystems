#!/usr/bin/env ruby
#github message. no quotes needed
commitMessage = ""
ARGV.each do|a|
  commitMessage += a
end
#puts "This is the message #{commitMessage}"
system "git init"
system "git add -A"
system "git commit -m #{commitMessage}"
system "git push origin master"