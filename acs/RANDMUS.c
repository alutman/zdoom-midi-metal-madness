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

//// START FUNCTIONS ////
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
    
  int OneZero = "0"; int TwoZeros = "00";
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

function int getMaxRandSong(void)
{
  int temp;
  int i;
  
  for(i = 1; i < MAX_SUPPORTED_SONGS; i++)
  {
    temp = getSongStr(i, SONGSTR_PREFIX); // Compare the string and the language text, if they're the same it's the last one.
    if(!CStrCmp(temp, StrParam(l:temp)))
    {
      return i - 1;
    }
  }
  
  return -1;
}

function void tapeChange(void)
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
    SetMusic(getSongStr(songId, SONGFILE_PREFIX), 0);
    return 0;
  }
  else { // Treating zero as meaning the default song
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
  return GetCvar(DEFAULT_MODE_CVAR) == 1;
}

function void setDefaultMode(int val) {
  SetCvar(DEFAULT_MODE_CVAR, val);
}

function int isValidSong(int songId)
{
  int infoId = getSongStr(songId, SONGSTR_PREFIX);
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
    int infoId = getSongStr(songId, SONGSTR_PREFIX);
    SetFont("SmallFont");
    if(bold) { // Bold when run on the server, notifies all players, non-bold is activator only
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

// Set playedSongs array values to zero
function void defaultPlayedSongs(int start) {
  int i;
  for (i = start; i < MAX_SUPPORTED_SONGS; i++) {
      playedSongs[i] = 0;
  }
}

//// END FUNCTIONS ////

//// START SONGINFO ////

// Tell clients the server's current song
Script 340 (void) NET
{
  int songId = getCurrentSong();
  showLocalInfo(songId);  
}

// Called by 'origsong' command
Script 341 (void) NET clientside
{
  if(isServerMode()) {
    RequestScriptPuke(340, 0, 0, 0); //Request the server parrot the songinfo
  }
  else {
    // Client side NET never leaves the local machine
    if(isDefaultMode()) {
      showInfo(0);
    }
    else {
      showInfo(getCurrentSong());  
    }
  }
}
//// END SONGINFO ////


//// START INIT ////
// SERVER
// Tell new players the currently playing song
Script 342 ENTER
{
  if(isServerMode())
  {
    Delay(35);
    showLocalInfo(getCurrentSong()); 
  }
}

// CLIENT
// Start a new song if we're a clientmode user. Start song has to be here
// as only ENTER has the CVAR for default mode needed to properly use script 390
//
// Side effect of using this method is that it will fire for all waiting MP players
// as soon as one of them joins. Can't find a way to properly link client map variables here
// to specific players
bool hasEntered = false; // Must run ONLY once
Script 331 ENTER CLIENTSIDE
{
  if(!isServerMode() && !hasEntered) // This will be using client side song history
  {
    if(!isDefaultMode()) { // Don't want to progress the nextSong if we're in default mode
      int songId = getNextSong();
      ACS_ExecuteWithResult(390, songId, 0, 1, 0); // Play song  
    }
  }
  hasEntered = true;
}


// SERVER
Script 344 OPEN
{
  if(!doneInit) { // Init array to zero
    defaultPlayedSongs(0);  
    doneInit = true;
  }
  if (isServerMode()) { // Start a song if we're a server just starting up
    int songId = getNextSong(); 
    ACS_Execute(390, 0, songId, 0, 1);
  }
}
// CLIENT
Script 345 OPEN clientside
{
  if(!doneInit) { // Init array to zero
    defaultPlayedSongs(0);  
    doneInit = true;
  }
}
//// END INIT ////


// LOGIC
// Play a song. This may be called from client side or server side depending on the setting
// songId: song to play
// curSongId: the currentSong ID to fall back on
// isStart: are we asking to play the first song of the level?
script 390 (int songId, int curSongId, int isStart) 
{
  if(!isStart) { // Shouldn't perform the change noise if this is the start
    silence();
    tapeChange();
    Delay(35); 
    // Delay means actual tics so this script will hang until the player is passing time
    // This means that the song playing is delayed until the fade into is done, hence
    // cannot be run in OPEN/ENTER properly
  }
  if(isStart && isDefaultMode()) {
    //Do nothing if default mode is set
  }
  else if(songId > 0) // Real song ID, play it
  {
    playSong(songId);
    Delay(35);
    showInfo(songId);
    setDefaultMode(0);
  }
  else if (songId == 0 && isDefaultMode()) { // Toggle default off, play the current song again
    playSong(curSongId);
    Delay(35);
    showInfo(curSongId);
    setDefaultMode(0);
  }
  else if(songId == 0 && !isDefaultMode()){ // Toggle default on, play the default song
    playSong(0);
    Delay(35);
    showInfo(0);
    setDefaultMode(1);
  }

}

//// START CONTROL ////

// LOGIC
// Perform various jukebox actions. Can run on client or server
script 346 (int mode)
{
  int songId;
  switch (mode)
  {
    case 0: // Next song
      songId = getNextSong();
      ACS_Execute(390, 0, songId, 0, 0);
      break;
    case 1: // Previous song
      songId = getPreviousSong();
      if(songId > 0) { // Don't do anything if there are no previous songs
        ACS_Execute(390, 0, songId, 0, 0);
      }
      break;
    case 2: // Default Song
      songId = getCurrentSong();
      if(songId == 0) {
        songId = getNextSong();
      }
      //!! Note the different parameter position. songId is in the curSongId slot
      //  we're actually asking to play song zero (default song)
      ACS_Execute(390, 0, 0, songId, 0);
      break;
  }
}

// SERVER
// 's_nextsong', 's_prevsong', 's_origsong' alias. Runs server side only
script 347 (int mode)
{
  if(isServerMode()) {
    ACS_Execute(346, 0, mode, 0, 0);  
  }
}

// CLIENT
// 'nextsong', 'prevsong', 'origsong' alias. Runs clientside & only on the requester machine
script 348 (int mode) NET clientside
{
  if(!isServerMode()) {
    ACS_Execute(346, 0, mode, 0, 0);  
  }
}

// LOGIC
// Specify an exact song to play. Can run on server or client
script 349 (int songId)
{

  if(songId > 0){ // Don't play default this way
    if(isValidSong(songId)) { // Do nothing if the songId is too great
      setDefaultMode(0);  // If we're manually selecting, we should move out of default mode

      // Write this next song to the history
      if(currentSongIndex+1 < MAX_SUPPORTED_SONGS) {
        currentSongIndex += 1;
        playedSongs[currentSongIndex] = songId;
        defaultPlayedSongs(currentSongIndex+1); // Empty out saved history for songs after this one
      }
      ACS_Execute(390, 0, songId, 0, 0); //Play song
    }
  }
}

//SERVER
// 's_song {ID}' alias. Runs server side only
script 350 (int songId) // Manually changing the song
{
  if(isServerMode()) {
    ACS_Execute(349, 0, songId, 0, 0);  
  }
}

//CLIENT
// 'song {ID}' alias. Runs clientside & only on the requester machine
script 351 (int songId) NET clientside // Manually changing the song
{
  if(!isServerMode()) {
    ACS_Execute(349, 0, songId, 0, 0);  
  }
}

//// END CONTROL ////