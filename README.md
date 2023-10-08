# Pirate Islands

Some islands, some pirates infesting the islands, a boat trying to safely navigate between the islands.

[Input file format](https://github.com/JamesBremner/PirateIslands/wiki/Input-format)

Input Example

<pre>
l isle1 isle2 6
l isle2 isle3 1
l isle2 isle4 2
l isle3 isle4 3
p isle3 6
p isle3 7
p isle3 8
r isle1 isle3

</pre>

Output

<pre>
at time 0 isle1
sailing sailing sailing sailing sailing sailing 
at time 6 arrive isle2
staying staying sailing 
at time 9 arrive isle3
</pre>
