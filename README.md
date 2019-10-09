# A simple regular expression matching program using NFA

This is a small regular expression program which could parse
and match simple regular expression.

The idea of NFA is learned from [1]. The implementation mainly
refer to Russ Cox's [Regular expression matching can be simple and fast][1].

[1]: <https://swtch.com/~rsc/regexp/regexp1.html>

# Plain Text

The program could matching plain text including all English alphabets
and Arabic numerals

# Parenthesis

The parenthesis could only be used to specify the priority. No capture is
implemented.

# Quantity and Alternation Character

The program support quantity characters including `*`, `?` and `+` and
alternation character `|`.

# Reference

[1] Thompson K. Programming Techniques: Regular expression search
algorithm[J]. Comm Acm, 1968, 11(6):419-422.
