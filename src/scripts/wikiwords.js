


function test(word) {
    print("test");
}


function findSoundUrl(word) {
    print("wikiwords.js findSoundUrl");
    
    downloader.download("http://en.wiktionary.org/wiki/" + word); 
}

function parseData(lines) {
    print("wikiwords.js parseData");
    for (i = 0; i < lines.length; i++) {
        line = lines[i];
        test = line.indexOf('<span class="IPA unicode audiolink">');
        if (test != -1) {
            startUrl = line.indexOf('a href="', test) + 8;
            endUrl = line.indexOf('"', startUrl);
            urlString = line.substring(startUrl, endUrl);
            downloader.urlFound(urlString);
            return;
        }
    }    
}

function init() {
    print("wikiwords.js init()");
    try {
        result = downloader["linesAvailable(const QStringList &)"].connect(parseData);
    } catch (e) {
        print(e);
    }
}

