This tool implements the method to compute N-gram IDF weights for all valid word N-grams in the given corpus (document set) [1][2].

# How to use

Requirement: C++11

Setup

    $ ./waf configure
    $ ./waf

Exact count (time consuming)

    $ bin/default/ngweight -w -s 0 < sample_text > sample_output

Approximate count (Approx10), min freq for valid N-gram = 5

    $ bin/default/ngweight -w -s 10 -t 5 < sample_text > sample_output2

Recommended approximation count is 50 (-s 50) or more based on the results [1].

# Input format

Text documents with delimiters.

Each input document has a header starting by ASCII code "0x02" and ending by "0x03". This header acts as a delimiter for documents.

# Output format

[id]    [len]   [gtf]   [df]    [sdf]   [term]

in TSV(Tab Separated Values) format.

* id: internal node id

* len: term length (number of words in N-gram, or N)

* gtf: global term frequency

* df: document frequency of N-gram

* sdf: document frequency of a set of words composing N-gram

* term: N-gram

# Processed data

Using English Wikipedia data (Oct. 1, 2013)

http://mljournalism.com/ngw/ngram.bz2

total number of documents |D| = 4,379,810

# Demo system

http://mljournalism.com/ngw/

# N-gram IDF weight

weight1 = log(|D|/sdf)

weight2 = log(|D| * df/sdf^2)

weight2 alone works well while weight1 may be useful as a part of TF-IDF scheme (requires further research on weight1).

# Key Term Detection Dataset

Testset is contained in directory "keytermdetection".

File "testset" contains first paragraphs of 1,678 Wikipedia articles.
One paragraph of an article is enclosed by \<doc\> and \</doc\>.

    <doc id="1482" title="Ask and Embla">
    In Norse mythology, Ask and Embla (from Old Norse "Askr ok Embla")—male and female respectively—were the first two humans, created by the gods. The pair are attested in both the Poetic Edda, compiled in the 13th century from earlier traditional sources, and the Prose Edda, written in the 13th century by Snorri Sturluson. In both sources, three gods, one of whom is Odin, find Ask and Embla and bestow upon them various corporeal and spiritual gifts. A number of theories have been proposed to explain the two figures, and there are occasional references to them in popular culture.
    </doc>

File "keyterms" contains key terms for each paragraph.
One line in TSV format corresponds to a paragraph.
The first number is doc id and the remains are key terms in it.

    1482	norse mythology odin	ask and embla	poetic edda	three	old norse	prose edda	snorri sturluson

# Reference

[1] Masumi Shirakawa, Takahiro Hara and Shojiro Nishio: IDF for Word N-grams, ACM Transactions on Information Systems (TOIS), Vol. 36, Issue 1, Article 5 (June 2017). Paper can be accessed through [author's web site](http://iwnsew.com).

[2] Masumi Shirakawa, Takahiro Hara and Shojiro Nishio: N-gram IDF: A Global Term Weighting Scheme Based on Information Distance, International World Wide Web Conference (WWW 2015), pp 960-970 (May 2015). [[paper](http://iwnsew.com/material/www2015/www2015paper.pdf)]

