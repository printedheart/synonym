Installation Instructions.

## Installing Synonym on Windows ##
  * Download [Windows package](http://synonym.googlecode.com/files/synonym-0.2-exe.zip).
  * Unpack the archive, which will result in the synonym folder. Inside the folder there will be "synonym.exe" file, "plugins" folder, and several other files. Do not move synonym.exe outside of the folder; if you want to move synonym in some other place move the whole "synonym" folder.
  * Download [database package](http://synonym.googlecode.com/files/wordnet30.1.tar.gz).
  * To open the archive you can use [Winrar](http://www.rarlab.com/download.htm).
  * When you unpack the archive you will see "wordnet30" file. For convenience you can place the database file in the "synonym" folder where you have "synonym.exe"
  * Run synonym.exe. If you put "wordnet30" file outside of "synonym" folder you will get a dialog to choose the database file.

## Installing Synonym on Linux ##

**To build:**
  * Make sure you have Qt >= Qt4.4
  * download
  * unpack( tar -zxvf synonym-0.2-src.tar.gz)
  * cd synonym
  * qmake
  * make
  * The executable should be in bin directory.

**To run:**
  * download database file
  * unpack(tar -zxvf wordnet30.1.tar.gz)
  * place wordnet30 file somewhere on the filesystem. During the first run a dialog will appear to select the database file.
  * Audio pronunciation works only if connected to the Internet. To enable go to Settings/Audio and install a script of your choice.