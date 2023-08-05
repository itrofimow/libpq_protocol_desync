# this rule replaces incoming CZ (Command completion, ReadyForQuery) with CE (Command completion, Error), thus dropping one ReadyForQuery packet
netsed tcp 5432 pg 5432 s/%43%00%00%00%0d%53%45%4c%45%43%54%20%31%00%5a%00%00%00%05%49/%43%00%00%00%0d%53%45%4c%45%43%54%20%31%00%45%00%00%00%6d%53%45%52%52%4f%52%00%56%45%52%52%4f%52%00%43%35%37%30%31%34%00%4d%63%61%6e%63%65%6c%69%6e%67%20%73%74%61%74%65%6d%65%6e%74%20%64%75%65%20%74%6f%20%73%74%61%74%65%6d%65%6e%74%20%74%69%6d%65%6f%75%74%00%46%70%6f%73%74%67%72%65%73%2e%63%00%4c%33%33%32%38%00%52%50%72%6f%63%65%73%73%49%6e%74%65%72%72%75%70%74%73%00%00/i 2>&1 >/dev/null &

./protocol_desync postgresql://postgres:postgres@localhost:5432/
