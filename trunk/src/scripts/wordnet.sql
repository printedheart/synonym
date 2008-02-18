

-- Get synsets for word.
SELECT word.wordid, synset.synsetid, pos, definition
FROM word, sense, synset
WHERE word.wordid = sense.wordid
AND sense.synsetid = synset.synsetid
AND lemma = 'word'

-- Get samples
SELECT sample from sample WHERE synsetid = ?

-- Get all words for synsets
SELECT word.wordid, lemma FROM word, sense, synset
WHERE word.wordid = sense.wordid
AND sense.synsetid = synset.synsetid
AND synset.synsetid IN();


-- Get all semantic links
SELECT linkdef.linkid, synset1id,  synset2id pos, definition
FROM semlinkref, linkdef, synset
WHERE semlinkref.linkid = linkdef.linkid
AND synset.synsetid = semlinkref.synset2id
AND semlinkref.synset1id IN


-- Get all lexical links.
SELECT synset1id, word1id, synset2id, word2id, name
FROM lexlinkref, linkdef
WHERE lexlinkref.linkid = linkdef.linkid
AND synset1id IN()



SELECT linkdef.linkid, synset1id,  synset2id pos, definition
FROM semlinkref, linkdef, synset
WHERE semlinkref.linkid = linkdef.linkid
AND synset.synsetid = semlinkref.synset2id
AND semlinkref.synset1id IN
(
SELECT synset.synsetid
FROM word, sense, sample, synset
WHERE word.wordid = sense.wordid
AND sense.synsetid = synset.synsetid
AND sample.synsetid = synset.synsetid
AND lemma = 'word'
);

SELECT lemma, name FROM word, sense, lexlinkref, linkdef, synset
WHERE word.wordid = lexlinkref.word2id
AND sense.synsetid = lexlinkref.synset1id
AND sense.synsetid = synset.synsetid;

select se1.rank, w2.lemma, sy1.definition, sy2.definition
from word w1 left join sense se1 on w1.wordid = se1.wordid
left join synset sy1 on se1.synsetid = sy1.synsetid
left join lexlinkref on sy1.synsetid = lexlinkref.synset1id and w1.wordid = lexlinkref.word1id
left join synset sy2 on lexlinkref.synset2id = sy2.synsetid
left join sense se2 on sy2.synsetid = se2.synsetid
left join word w2 on se2.wordid = w2.wordid where w1.lemma = 'bad' order by se1.rank asc;


select se1.rank, w2.lemma, sy1.definition, sy2.definition, ldef.name, sy1.pos, sy2.pos
from word w1
left join sense se1 on w1.wordid = se1.wordid
join synset sy1 on se1.synsetid = sy1.synsetid
join lexlinkref lref on sy1.synsetid = lref.synset1id and w1.wordid = lref.word1id
join synset sy2 on lref.synset2id = sy2.synsetid
join sense se2 on sy2.synsetid = se2.synsetid
join word w2 on se2.wordid = w2.wordid
join linkdef ldef on lref.linkid = ldef.linkid
where w1.lemma = 'b' order by se1.rank asc;

























