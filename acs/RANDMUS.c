#library "RANDMUS"
#include "zcommon.acs"

#define MAX_SUPPORTED_SONGS 999
#define SERVER_MODE_CVAR "mus_servermode"
#define DEFAULT_MODE_CVAR "mus_default"
#define SONGSTR_PREFIX "JUKEBOX_"
#define SONGFILE_PREFIX "SONG"


global int 1:currentSongIndex;
global int 2:playedSongs[];
global bool 3:doneInit;
global bool 4:hasEntered;

function int cstrcmp(int s1, int s2)
{
  int i = 0;
  int c1, c2;
  
  while(true) {
    c1 = GetChar(s1, i); c2 = GetChar(s2, i);
    
    if((!c1 && !c2) || (c1 != c2)) { break; }
    
    i++;
  }
  
  return c1 - c2;
}

function int getSongStr(int SongNumber, int PrefixString)
{
    // if the song amount ever goes over 1000, uncomment everything but this line ...
  int OneZero = "0"; int TwoZeros = "00"; // int ThreeZeros = "000";
  int OutputString;
  if(SongNumber <= 9)
  { // if it's less than 10 (1 digit, two zeros)
    OutputString = StrParam(s:PrefixString, s:TwoZeros, d:SongNumber); // concatenate the prefix, two zeros, and the number
  }
  else if(SongNumber <= 99)
  { // if it's less than 100 (2 digits, one zero)
    OutputString = StrParam(s:PrefixString, s:OneZero, d:SongNumber); // concatenate the prefix, one zero, and the number
  }
  else
  { // if it's over 100
    OutputString = StrParam(s:PrefixString, d:SongNumber); // concatenate the prefix and the number
  }
  
  return OutputString;
}

function int getSongInfo(int SongNumber)
{
  return getSongStr(SongNumber, SONGSTR_PREFIX);
}

function int getSongFile(int SongNumber)
{
  return getSongStr(SongNumber, SONGFILE_PREFIX);
}

function int getMaxRandSong(void)
{
  int temp;
  int i;
  
  for(i = 1; i < MAX_SUPPORTED_SONGS; i++)
  { // Please do not use more than 9999 songs.
    temp = getSongInfo(i); // Compare the string and the language text, if they're the same it's the last one.
    if(!CStrCmp(temp, StrParam(l:temp)))
    {
      return i - 1;
    }
  }
  
  return -1;
}

function void playTapeChange(void)
{
  AmbientSound("music/shift",127);
}
function void silence(void)
{
  SetMusic("silence", 0);
}

function int playSong(int songId) 
{
  if(songId > 0) 
  {
    SetMusic(getSongFile(songId), 0);
    return 0;
  }
  else {
    SetMusic("*", 0);
    return 0;
  }
  return 1;
}

function int getCurrentSong(void)
{
  return playedSongs[currentSongIndex];  
}

function int getNextSong(void) 
{
  int songId;
  // Too much history, not bothering to shift array
  if(currentSongIndex+1 >= MAX_SUPPORTED_SONGS) 
  {
    //too much, i give up
    return getRandomSongId();
  }

  // First song case
  if(playedSongs[currentSongIndex] == 0) 
  {
    songId = getRandomSongId();
    playedSongs[currentSongIndex] = songId;
    return songId;
  }

  // Traversing a previously generated song
  if(playedSongs[currentSongIndex+1] > 0) 
  {
    currentSongIndex += 1;
    return playedSongs[currentSongIndex];
  }
  else // Need to generate a new song
  {
    songId = getRandomSongId();
    playedSongs[currentSongIndex+1] = songId;
    currentSongIndex += 1;
    return songId;
  }
  return 0;
}

function int getPreviousSong(void) {
  if(currentSongIndex-1 >= 0) {
    currentSongIndex -= 1;
    return playedSongs[currentSongIndex];
  }
  return 0;
}

function int isServerMode(void) {
  return GetCvar(SERVER_MODE_CVAR) == 1;
}

function int isDefaultMode(void) {
  return !isServerMode() && GetCvar(DEFAULT_MODE_CVAR) == 1;
}

function void setDefaultMode(int val) {
  if(!isServerMode()) {
    SetCvar(DEFAULT_MODE_CVAR, val);  
  }
  
}

function int isValidSong(int songId)
{
  int infoId = getSongInfo(songId);
  return CStrCmp(infoId, StrParam(l:infoId));
}

function int showInfo(int songId) 
{
  return showInfoMode(songId, true);
}
function int showLocalInfo(int songId) 
{
  return showInfoMode(songId, false);
}
function int showInfoMode(int songId, bool bold) 
{
  if(isValidSong(songId))
  {
    int infoId = getSongInfo(songId);
    SetFont("SmallFont");
    if(bold) {
      HudMessageBold(l:infoId; HUDMSG_FADEINOUT | HUDMSG_LOG, 153, CR_WHITE, 0.1, 0.8, 3.0, 0.5, 1.0);
    }
    else {
      HudMessage(l:infoId; HUDMSG_FADEINOUT | HUDMSG_LOG, 153, CR_WHITE, 0.1, 0.8, 3.0, 0.5, 1.0);     
    }
    
  }
  return 0;
}

function int getRandomSongId(void) {
  int randSong = 0;
  randSong = random(1, getMaxRandSong());
  return randSong;
}

function void defaultPlayedSongs(int start) {
  int i;
  for (i = start; i < MAX_SUPPORTED_SONGS; i++) {
      playedSongs[i] = 0;
  }
}

// START SONGINFO
Script 340 (void) NET
{
  int songId = getCurrentSong();
  ACS_Execute(341, 0, songId, 0, 0);  
}
Script 341 (int songId, int serverMode) NET clientside
{
  if(isServerMode()) {
    showLocalInfo(songId);  
  }
  else {
    if(isDefaultMode()) {
      showLocalInfo(0);
    }
    else {
      showLocalInfo(getCurrentSong());  
    }
  }
}
// END SONGINFO

// Display the songinfo when the player enters
Script 342 ENTER clientside
{

  // if(isServerMode()) {
  //   RequestScriptPuke(340, 0, 0, 0);
  // }
  // else {
  //   ACS_Execute(340, 0, 0, 0, 0);
  // }
  if(!isServerMode() && !isDefaultMode()) {
    silence();
    playSong(getCurrentSong());
    ACS_Execute(340, 0, 0, 0, 0);
  }
  else if(!isServerMode() && isDefaultMode())  {
    getPreviousSong();
  }
  else {
    RequestScriptPuke(340, 0, 0, 0);
  }
}

// START INIT
Script 344 OPEN
{
  if(isServerMode())
  {
    if(!doneInit) {
      defaultPlayedSongs(0);  
      doneInit = true;
    }
    silence();
    int songId = getNextSong();
    playSong(songId);
  }
}
Script 345 OPEN clientside
{
  if(!isServerMode())
  {
    if(!doneInit) {
      defaultPlayedSongs(0);  
      doneInit = true;
    }
    //silence()
    int songId = getNextSong();
    // playSong(songId);
  }
}
// END INIT

// START CONTROL
script 346 (int mode)
{
  int songId;
  switch (mode)
  {
    case 0: // Next song
      setDefaultMode(0);
      silence();
      playTapeChange();
      Delay(35);
      songId = getNextSong();
      playSong(songId);
      Delay(35);
      showInfo(songId);
      break;
    case 1: // Previous song
      setDefaultMode(0);
      songId = getPreviousSong();
      if(songId > 0) { // Don't stop the music if there are no previous songs
        setDefaultMode(0);
        silence();
        playTapeChange();
        Delay(35);
        playSong(songId);
        Delay(35);
        showInfo(songId);
      }
      break;
    case 2: // Default Song
      silence();
      playTapeChange();
      Delay(35);
      if(isDefaultMode())
      {
        songId = getCurrentSong();
        if(songId == 0) {
          songId = getNextSong();
        }
        playSong(songId);
        Delay(35);
        showInfo(songId);
        setDefaultMode(0);
      }
      else {
        playSong(0);
        Delay(35);
        showInfo(0);
        setDefaultMode(1);
      }  
      break;
  }
}

script 347 (int mode)
{
  if(isServerMode()) {
    ACS_Execute(346, 0, mode, 0, 0);  
  }
}
script 348 (int mode) NET clientside
{
  if(!isServerMode()) {
    ACS_Execute(346, 0, mode, 0, 0);  
  }
}
//END CONTROL

// START MANUAL SONG
script 349 (int songId)
{

  if(songId > 0){
    if(isValidSong(songId)) {
      setDefaultMode(0);
      if(currentSongIndex+1 < MAX_SUPPORTED_SONGS) {
        currentSongIndex += 1;
        playedSongs[currentSongIndex] = songId;
        defaultPlayedSongs(currentSongIndex+1); // Empty out saved history for songs after this one
      }
      silence();
      playTapeChange();
      Delay(35);
      playSong(songId);
      Delay(35);
      showInfo(songId);
    }
  }
}
script 350 (int songId) // Manually changing the song
{
  if(isServerMode()) {
    ACS_Execute(349, 0, songId, 0, 0);  
  }
}

script 351 (int songId) NET clientside // Manually changing the song
{
  if(!isServerMode()) {
    ACS_Execute(349, 0, songId, 0, 0);  
  }
}

// END MANUAL SONG