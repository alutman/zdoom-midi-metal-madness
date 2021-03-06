Midi Metal Madness
===================
Description
-----------
Modification of '"Thats what I call MIDI", 2nd unofficial version
ZDoom plugin that plays MIDI versions of metal songs instead of the default Doom tracks. Songs are randomly selected from a list of 650.

Settings
--------
* (server) mus_servermode [0|1] : Disable/Enable server music mode. Only the server can change the track, everyone hears the same song. Client mode will give everyone their own jukebox to control and listen to.
* (client) mus_default [0|1] : Play the default music on level start.

Commands
-------
All client side ones are keybindable

* songinfo: Display the currently playing song. Works with client and server mode.

#### Client mode (mus_servermode 0)
* nextsong: Play the next song. If there's no history at this point, it will be a random song 
* prevsong: Play the previous song. Does nothing if there's no previous
* origsong: Toggles mus_default, playing default music or the current song as appropriate

#### Server mode  (mus_servermode 1)
* s_nextsong: Like Client but run on the server. Will forward the song in the same way for all players 
* s_prevsong: Like Client but run on the server
* s_origsong: Like Client but run on the server


Clarification
-------------
Thats what I call MIDI, unofficial V3.

Modification of '"Thats what I call MIDI", 2nd unofficial version' which was an update of 150 tracks over the Official "Thats what I call MIDI".
This version is also very unofficial and in no way related to the original author (Terminus) or the updater (Truvix). 
Just a fan version with some extra features I wanted


Licensing
---------
I don't know the license of all the MIDI files so I haven't included them in this repo yet.
If you download a previous version from elsewhere and place the `music` & `sounds` folders in this one, it should work fine.


TODO
* Filter genres option
* Option to play next track instead of looping
* Search tracks by artist/name/genre
* Better psudorandom. Store history so you don't hear the same song twice until you hear them all. Persist over shutdowns


CREDITS TO
----------
TerminusEst13 (Original release)
Bloax (Song gathering, info checking)
TheToxicAvenger (Song gathering, info checking)
Mike12 (Song gathering)
CookieJar (Song gathering)
Ryan Cordell (Fixing the "song xxx" bug)
Marrub (Converting all the ACS to use strParam instead of an array, clientsiding the music as well)
Truvix (Song Gathering, info checking)
Lex (Track history, code refactor, server mode)


Changelog
----------

UNOFFICIAL EXPANSION V3
=======================
- Fixed music not playing in new GZDoom
- Gave all music files an extension
- Added track history with previous track button
- Show track info button
- Removed code surrounding Zandorum random number generator as it seems fine in 3.0
- Added server mode to have all players hear the same song

UNOFFICIAL EXPANSION V2
===========================
- 149 new songs
- Yep, just songs

FROM V3.2 TO V3.5
===========================
- 25 more songs.
- SpawnForced was introduced into Zandronum since the update. ZDoom check
  rewritten with this in mind.
- nomusic was a server cvar instead of a user cvar. This has been fixed.
- The DemonSteele check would break at seemingly random times and summon a
  Revenant to punch the player. This has been fixed.


FROM V3 TO V3.2
===========================
- 25 more songs.
- Conversion of cvar handling from ConsoleCommand to cvarinfo.


FROM V2 TO V3:
===========================
- 75 more songs!
- Judas Priest's Tyrant had a random beep around the 0:56-ish mark. This has been
  fixed.
- Dark ambient music has been removed and replaced. This is a pack for jamming to
  super-cheesy and tinny music, not getting 2spook'd.
- White Zombie's Temple of the Bull wasn't a song we were able to find any info
  on. We're not even sure it's actually a real song. It was replaced with a new
  White Zombie song, Black Sunshine.
- It's now possible to change exactly to the song you want. Input "song" and then
  a number in the console, like "song 142", and it'll swap to that exact song.
- All song info has been given tan and white coloring for easier differentiation.
- Thanks to Marrub's incredible work, even the song amount now is clientsided.
  People are now able to add, remove, and adjust songs as they want without any
  issues in joining servers!
- Hitting "Next Song" and manually selecting a song no longer requires the
  norandommusic cvar to be set to 0.
- Corrected some mistaken song information:
  ^- Barathrum's Warmetal is off the album Infernal.
  ^- Linkin Park's Crawling is off the album Hybrid Theory.
  ^- Battlelore's Stormmaster was renamed to the proper Swordmaster.
  ^- Song 186 was credited to Cannical Corpse instead of Cannibal Corpse.
  ^- Sanctuary's Die for My Sins and Taste Revenge are Thrash Metal.


FROM V1c TO V2:
===========================
- 100 more songs!
- Removed and replaced Rammstein's Engel with Feuer Frei, due to Engel messing up
  in-game and playing ring tones instead.
- Added a "default song" button to play the level's standard music.
- Moved all song info to LANGUAGE, for people to edit at their leisure if they
  replace the music.
- Messages now no longer overlap and will replace each other.
- Necroticism - Descanting the Insalubrious had an unreadable symbol that was
  refusing to be read properly.
- Corrected some mistaken song information:
  ^- A Succubus In Rapture was not by set_abominae, it was by Dimmu Borgir.
  ^- Rammstein has identified as Neue Deutsche Harte.
  ^- Annihilator has never identified their genre as groove metal.
  ^- Biohazard has identified as Hardcore.
  ^- Ea, Lord of the Depths was on the album Burzum.
  ^- Alice in Chains is decisively Grunge.
  ^- The Midnight Carnival is Speed, not Power.
  ^- Children of Bodom's songs have been labeled Melodic Death.
  ^- Fear Factory is Industrial Metal, not Death Metal.
  ^- Graveworm is now Melodic Black Metal.
  ^- Hexen is now Thrash Metal.
  ^- Lamb of God is now metalcore.
  ^- Refuse/Resist was on Chaos A.D., not Confuse A.D.
  ^- Reviewers have identified Celtic Frost's To Mega Therion (not Thyrion)
     album as Death Metal.