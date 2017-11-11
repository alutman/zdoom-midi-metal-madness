#library "RANDMUS"
#include "zcommon.acs"

// #include "commonFuncs.h" // Even in making a silly music randomizer mod, I require ijon's funcs.
// commonFuncs.h wouldn't compile in acc156, ported over the necessary stuff to a smaller file
#include "util.h"

int MusicCurrentSong;

#define SONGSTR_INFO 1
#define SONGSTR_SONG 0

#define MAX_SUPPORTED_SONGS 9999

int playedSongs[MAX_SUPPORTED_SONGS];
int currentSongIndex = 0;
int lastSongIndex = 0;

function int getSongStr(int SongNumber, int Type)
{
  // if the song amount ever goes over 1000, uncomment everything but this line ...
  int OneZero = "0"; int TwoZeros = "00"; // int ThreeZeros = "000";
  int PrefixString;
  int OutputString;
  
  if(Type == SONGSTR_INFO)
  {
    PrefixString = "JUKEBOX_";
  }
  else if(Type == SONGSTR_SONG)
  {
    PrefixString = "Song";
  }
  else
  {
    PrefixString = "OhGodThisIsAnError";
  }
  
  // ... and remove these blocks
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

function int getMaxRandSong(void)
{
  int temp;
  int i;
  
  for(i = 1; i < MAX_SUPPORTED_SONGS; i++)
  { // Please do not use more than 9999 songs.
    temp = getSongStr(i, SONGSTR_INFO); // Compare the string and the language text, if they're the same it's the last one.
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
    SetMusic(getSongStr(songId, SONGSTR_SONG),0);
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

function int showInfo(int songId) {
  if(GetCvar("noinfo") == 0 && songId > 0)
  {
      int infoId = getSongStr(songId, SONGSTR_INFO);
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
  int randomhack;
  int lastmus;
  
  if (GetCvar("mus_runninginzdoom") == 1)
  {
    randSong = random(1, getMaxRandSong());
  }
  else
  {
    // Zandronum is funny about this. Online, it generates the same random seed every
    // time, even though that's the opposite of what you'd expect from a random call.
    // I have to use a workaround to generate pseudo-random behavior.
    // But it's better than dealing with ZDoom's barely-functional slapshod netcode!
    
    // [marrub] The reason this happens is the C/S code wants to be extra-deterministic
    // so everyone gets the same (P)RNG. This is a guess. Please don't hit me, Dusk.
    // Also on ZDoom everyone knows they have the same PRNG so there's no worry there.
      
    randomhack = defaultCVar("mus_cl_randomhack",   0);
    lastmus    = defaultCVar("mus_cl_lastmusic",    0) - 1;
    int i;
    for (i = 0; i < randomhack; i++) { random(0, 1); }
    randomhack = random(0, 500);
    SaveCVar("mus_cl_randomhack", randomhack);
    
    do { i = random(1, getMaxRandSong()); }
    while (i == lastmus);
    randSong = i;
    SaveCVar("mus_cl_lastmusic", i + 1);
  }
  return randSong;
}

function void defaultPlayedSongs(int start) {
  int i;
  for (i = start; i < MAX_SUPPORTED_SONGS; i++) {
      playedSongs[i] = 0;
  }
}

script 345 OPEN
{
  int r = unusedTID(37000, 47000);

  delay(1);
  if (SpawnForced("SpeakerIcon",0,0,0,r,0))
  { Thing_Remove(r); SetCVar("mus_runninginzdoom",1); }
  else
  { SetCVar("mus_runninginzdoom",0); }
}

Script 346 OPEN NET clientside
{
  if(GetCvar("nomusic") == 0)
  {
    defaultPlayedSongs(0);
    int songId = getNextSong();
    playSong(songId);
    Delay(35);
    showInfo(songId);
  }
}

script 347 OPEN clientside
{
  delay(1);
  
  if(GetCvar("mus_runninginzdoom") == 0) // No option to disable on ZDoom because I'm pretty sure the only way this'll
  {                                      // be happening is when you manually load it, ergo want to listen to it...
    if(!GetCvar("nomusic"))
    {
      ConsoleCommand("set nomusic 0");
      ConsoleCommand("archivecvar nomusic");
    }
    
    if(!GetCvar("noinfo"))
    {
      ConsoleCommand("set noinfo 0");
      ConsoleCommand("archivecvar noinfo");
    }
  }
}


script 348 (int mode) NET clientside
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
      showInfo(MusicCurrentSong);
      break;
    case 3: // Hitting "Default Song".
      SetMusic("silence");
      LocalAmbientSound("music/shift",127);
      Delay(35);
      SetMusic("*",0);
      break;
  }
}

script 349 (int songId) NET clientside // Manually changing the song
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
