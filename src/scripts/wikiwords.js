

function findSoundUrl(word) {
    downloader.download("http://en.wiktionary.org/wiki/" + word, "parseData"); 
}

function parseData(lines) {
   // print("parseData");
    for (i = 0; i < lines.length; i++) {
        line = lines[i];
        test = line.indexOf('<span class="unicode audiolink">');
        if (test != -1) {
            startUrl = line.indexOf('a href="', test) + 8;
            endUrl = line.indexOf('"', startUrl);
            urlString = line.substring(startUrl, endUrl);
            downloader.urlFound(urlString);
            return;
        }
    }    
}

function init() {}

