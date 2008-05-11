


function parseMainPage(lines) {
    for (i = 0; i < lines.length; i++) {
        line = lines[i];
        test = line.indexOf("javascript:popWin('/cgi-bin/audio.pl");
        
        if (test != -1) {
            try {
            downloader["linesAvailable(const QStringList &)"].disconnect(parseMainPage); 
            startIndex = line.indexOf("/", test);
            endIndex = line.indexOf("')", test);
            urlPart = line.substring(startIndex, endIndex);
            
            newUrl = "http://www.merriam-webster.com" + urlPart;
            downloader["linesAvailable(const QStringList &)"].connect(parseSoundPage);
            downloader.download(newUrl);
            } catch (e) {
                print(e);
            }
        }       
        
    }
}

function parseSoundPage(lines) {
    for (i = 0; i < lines.length; i++) {
        line = lines[i];
        test = line.indexOf("Click here to listen with your default audio player"); 
        if (test != -1) {
            startIndex = line.indexOf("http");
            endIndex = line.indexOf("wav");
            soundUrl = line.substring(startIndex, endIndex + 3);
            downloader["linesAvailable(const QStringList &)"].disconnect(parseSoundPage);
            downloader.urlFound(soundUrl);
            return;    
        }
    }
}

function findSoundUrl(word) {
    downloader["linesAvailable(const QStringList &)"].connect(parseMainPage);
    downloader.download("http://www.merriam-webster.com/dictionary/" + word); 
}


function init() {}