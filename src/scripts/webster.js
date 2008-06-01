


function parseMainPage(lines) {
    for (i = 0; i < lines.length; i++) {
        var line = lines[i];
        var test = line.indexOf("javascript:popWin('/cgi-bin/audio.pl");

        if (test != -1) {
            try {
            downloader["linesAvailable(const QStringList &)"].disconnect(parseMainPage);
            var startIndex = line.indexOf("/", test);
            var endIndex = line.indexOf("')", test);
            var urlPart = line.substring(startIndex, endIndex);

            var newUrl = "http://www.merriam-webster.com" + urlPart;
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
        var line = lines[i];
        var test = line.indexOf("Click here to listen with your default audio player");
        if (test != -1) {
            var startIndex = line.indexOf("http");
            var endIndex = line.indexOf("wav");
            var soundUrl = line.substring(startIndex, endIndex + 3);
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

