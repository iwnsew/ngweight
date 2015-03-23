# How to use

    $ ./waf configure
    $ ./waf
    $ bin/default/ngweight -w < sample_text > sample_output

# Output format

[internal node id] \t [term length] \t [gtf: global term freq] \t [df: document freq] \t [udf: unigram document freq] \t [term]

# Processed data

Using English Wikipedia data (Oct. 1, 2013)

http://mljournalism.com/ngw/ngram.bz2

# Demo system

http://mljournalism.com/ngw/

# N-gram IDF weight

weight = log(D * df/udf^2)

# Keyphrase Datasets

They will be released soon.

# Reference

Masumi Shirakawa, Takahiro Hara and Shojiro Nishio: N-gram IDF: A Global Term Weighting Scheme Based on Information Distance, International World Wide Web Conference (WWW 2015) (May 2015, to appear). [draft](http://iwnsew.com/material/www2015/www2015paper.pdf)

# License

Copyright (c) 2014 Masumi Shirakawa All Rights Reserved.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
