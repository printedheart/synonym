


var searchWord;

function parseMainPage(lines) {
    var re = new RegExp("<B>\\d\\) <A HREF=\"(http:\/\/www\\.bartleby\\.com.+)\">" + searchWord + "\\s?\\d?\\.", "i");    
    for (i = 0; i < lines.length; i++) {
        var line = lines[i];
        var result = line.match(re);
        if (result) {
          //  print("Match " + result);
            try {
                var newUrl = result[1];
                downloader.download(newUrl, "parseResultPage");
                break;
            } catch (e) {
                print(e);
            }
        }
    }
}

function parseResultPage(lines) {
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i];
        var test = line.indexOf(".wav");
        if (test != -1) {
            for (var j = test; j > 0; j--) {
                if (line.charAt(j - 1) == "\"") {
                    var soundUrl = line.substring(j, test + 4);
                    downloader.urlFound("http://www.bartleby.com" + soundUrl);
                    return;
                }
            }
        }
    }
}

function findSoundUrl(word) {
    searchWord = word;
    downloader.download("http://www.bartleby.com/cgi-bin/texis/webinator/ahdsearch?search_type=enty&query=" + word + "&db=ahd&Submit=Search", "parseMainPage");
}


function init() {}

