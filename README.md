# How to use

    $ ./waf configure
    $ ./waf
    $ bin/default/ngweight -w < sample_text > sample_output

# Output format

[internal node id] [term length] [gtf: global term freq] [df: document freq of n-gram] [sdf: document freq for a set of words consisting n-gram] [term (n-gram)]

in TSV format.

# Processed data

Using English Wikipedia data (Oct. 1, 2013)

http://mljournalism.com/ngw/ngram.bz2

# Demo system

http://mljournalism.com/ngw/

# N-gram IDF weight

weight1 = log(D/sdf)

weight2 = log(D * df/sdf^2)

weight2 alone works well while weight1 may be useful as a part of TF-IDF scheme.

# Key Term Detection Datasets

Testset is contained in directory "keytermdetection".

File "testset" contains first paragraphs of 1,678 Wikipedia articles.
One paragraph of an article is enclosed by <doc> and </doc>.

    <doc id="1482" title="Ask and Embla">
    In Norse mythology, Ask and Embla (from Old Norse "Askr ok Embla")—male and female respectively—were the first two humans, created by the gods. The pair are attested in both the Poetic Edda, compiled in the 13th century from earlier traditional sources, and the Prose Edda, written in the 13th century by Snorri Sturluson. In both sources, three gods, one of whom is Odin, find Ask and Embla and bestow upon them various corporeal and spiritual gifts. A number of theories have been proposed to explain the two figures, and there are occasional references to them in popular culture.
    </doc>

File "keyterms" contains key terms for each paragraph.
One line corresponds to a paragraph.
The first number is doc id and the rests are key terms in it.

    1482	norse mythology odin	ask and embla	poetic edda	three	old norse	prose edda	snorri sturluson

# Reference

Masumi Shirakawa, Takahiro Hara and Shojiro Nishio: N-gram IDF: A Global Term Weighting Scheme Based on Information Distance, International World Wide Web Conference (WWW 2015) (May 2015, to appear). [[draft](http://iwnsew.com/material/www2015/www2015paper.pdf)]

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
