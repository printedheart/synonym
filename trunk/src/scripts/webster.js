


function parseMainPage(lines) {
    for (i = 0; i < lines.length; i++) {
        var line = lines[i];
        var test = line.indexOf("javascript:popWin('/cgi-bin/audio.pl");
        if (test != -1) {
            try {
            var startIndex = line.indexOf("/", test);
            var endIndex = line.indexOf("')", test);
            var urlPart = line.substring(startIndex, endIndex);
            var newUrl = "http://www.merriam-webster.com" + urlPart;
            downloader.download(newUrl, "parseSoundPage");
            break;
            } catch (e) {
                print(e);
            }
        }

    }
}

function parseSoundPage(lines) {
    for (i = 0; i < lines.length; i++) {
        var line = lines[i];
        var test = line.indexOf("Click here to listen with your default audio player");
        if (test != -1) {
            var startIndex = line.indexOf("http");
            var endIndex = line.indexOf("wav");
            var soundUrl = line.substring(startIndex, endIndex + 3);
            downloader.urlFound(soundUrl);
            return;
        }
    }
}

function findSoundUrl(word) {
    downloader.download("http://www.merriam-webster.com/dictionary/" + word, "parseMainPage");
}


function init() {}

