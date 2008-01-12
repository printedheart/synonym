from PythonQt import *

import sys
sys.path.append('/usr/lib/python2.5/site-packages')


from nltk.wordnet import *


#class WordGraph:
    #def addWordNode(self, word):
        #print word
        
    #def addMeaningNode(self, nodeId, gloss, pos):
        #print nodeId
            
    #def addEdge(self, nodeId1, nodeId2, relationType=''):
        #print nodeId1, nodeId2
                

    #def setCentralNode(self, nodeId):
        #print "set central node", nodeId
        
        
        
wordGraph = None
        
def createGraph(word):
    global wordGraph
    wordGraph = WordGraph()
    wordGraph.addWordNode(word)
    wordGraph.setCentralNode(word)
    for dict in Dictionaries.values():
        loadDictionary(dict, word, wordGraph)
        

def loadDictionary(dict, word, wordGraph):
    try:
        w = dict[word]
        loadSynsets(word, w.synsets(), 0, wordGraph)       
    except KeyError, detail:
            print detail
    except:
        print "error"        
        
        
        
def loadSynsets(searchWord, synsets, depth, wordGraph):
    """ load synsets for search word.
    """
    loadedSynsets = set()
    for synset in synsets:
        if not hasattr(synset, 'gloss') or synset.gloss in loadedSynsets:
            continue
        
        loadedSynsets.add(synset.gloss)
            
        synsetId = synset.pos + str(id(synset))
        wordGraph.addMeaningNode(synsetId, synset.gloss, synset.pos)
        wordGraph.addEdge(searchWord, synsetId)
        for word in synset.words:
            wordGraph.addWordNode(word)
            wordGraph.addEdge(synsetId, word)
            
        relations = synset.relations
        for relationType, relationSynsets,  in synset.relations().iteritems():
            for relSynset in relationSynsets:
                if (hasattr(relSynset, 'gloss') and relSynset.gloss not in loadedSynsets):
                    relSynsetId = relSynset.pos + str(id(relSynset))
                    wordGraph.addMeaningNode(relSynsetId, relSynset.gloss, relSynset.pos)
                    success = wordGraph.addEdge(synsetId, relSynsetId, relationType)
                    if success:
                        for rword in relSynset.words:
                            wordGraph.addWordNode(rword)
                            wordGraph.addEdge(relSynsetId, rword)
                        
                        
