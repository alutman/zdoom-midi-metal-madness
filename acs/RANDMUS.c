#library "RANDMUS"
#include "zcommon.acs"

#define MAX_SUPPORTED_SONGS 999
#define NO_MUSIC_CVAR "mus_nomusic"
#define NO_INFO_CVAR "mus_noinfo"
#define SONGSTR_PREFIX "JUKEBOX_"
#define SONGFILE_PREFIX "SONG"

int playedSongs[MAX_SUPPORTED_SONGS];
int currentSongIndex = 0;

int MusicCurrentSong;

function int cstrcmp(int s1, int s2)
{
  int i = 0;
  int fuck, acs;
  
  while(true) {
    fuck = GetChar(s1, i); acs = GetChar(s2, i);
    
    if((!fuck && !acs) || (fuck != acs)) { break; }
    
    i++;
  }
  
  return fuck - acs;
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

function int playSong(int songId) {
  if(songId > 0) 
  {
    SetMusic(getSongFile(songId),0);
    MusicCurrentSong = songId;
    return 0;
  }
  return 1;
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

function int forceShowInfo(int songId) 
{
  return showInfoCond(songId, true);
}

function int showInfo(int songId) 
{
  return showInfoCond(songId, GetCvar(NO_INFO_CVAR) == 0);
}

function int showInfoCond(int songId, bool condition) 
{
  if(condition && songId > 0)
  {
      int infoId = getSongInfo(songId);
      if(CStrCmp(infoId, StrParam(l:infoId)))
      {
        SetFont("SmallFont");
        hudmessage(l:infoId; HUDMSG_FADEINOUT | HUDMSG_LOG, 153, CR_WHITE, 0.1, 0.8, 3.0, 0.5, 1.0);
      }
      return 0;
  }
    return 1;
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

Script 346 ENTER clientside
{
  if(GetCvar(NO_MUSIC_CVAR) == 0)
  {
    defaultPlayedSongs(0);
    int songId = getNextSong();
    playSong(songId);
    Delay(35);
    showInfo(songId);
  }
}

script 348 (int mode) NET clientside
{
  if(GetCvar(NO_MUSIC_CVAR) == 0)
  {
    int songId;
    switch (mode)
    {
      case 0: // Next song
        SetMusic("silence");
        LocalAmbientSound("music/shift",127);
        Delay(35);
        songId = getNextSong();
        playSong(songId);
        Delay(35);
        showInfo(songId);
        break;
      case 1: // Previous song
        songId = getPreviousSong();
        if(songId > 0) { // Don't stop the music if there are no previous songs
          SetMusic("silence");
          LocalAmbientSound("music/shift",127);
          Delay(35);
          playSong(songId);
          Delay(35);
          showInfo(songId);
        }
        break;
      case 2: // Show Info
        forceShowInfo(MusicCurrentSong);
        break;
      case 3: // Hitting "Default Song".
        SetMusic("silence");
        LocalAmbientSound("music/shift",127);
        Delay(35);
        SetMusic("*",0);
        break;
    }
  }
  else {
    Log(s:StrParam(s:NO_MUSIC_CVAR, s:" is enabled"));
  }
}

script 349 (int songId) NET clientside // Manually changing the song
{
  if(GetCvar(NO_MUSIC_CVAR) == 0)
  {
    if(songId > 0){
      if(currentSongIndex+1 < MAX_SUPPORTED_SONGS) {
        currentSongIndex += 1;
        playedSongs[currentSongIndex] = songId;
        defaultPlayedSongs(currentSongIndex+1); // Empty out saved history for songs after this one
      }
    }
    playSong(songId);
    showInfo(songId);
  }
  else {
    Log(s:StrParam(s:NO_MUSIC_CVAR, s:" is enabled"));
  }
}
