


function parseMainPage(lines) {
    for (i = 0; i < lines.length; i++) {
        var line = lines[i];
        var test = line.indexOf("return au");
        if (test != -1) {
            try {
                var startIndex = test + "return au".length + 2;
                var endIndex = line.indexOf("')", test);
                var parts = line.substring(startIndex, endIndex).split("', '");

                var newUrl = "http://www.merriam-webster.com/cgi-bin/audio.pl?" + parts[0] + "=" + parts[1];
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
            var endIndex = line.indexOf("\">Click here");
            var soundUrl = line.substring(startIndex, endIndex);
            downloader.urlFound(soundUrl);
            return;
        }
    }
}

function findSoundUrl(word) {
    downloader.download("http://www.merriam-webster.com/dictionary/" + word, "parseMainPage");
}


function init() {}

