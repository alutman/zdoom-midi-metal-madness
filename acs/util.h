// [marrub] ijon's strcmp doesn't work, hell if I know why
// so I stol-- er, liberated -- some code from the standard C library
/* for(; *s1 && *s2; ++s1, ++s2)
   {
      if(*s1 != *s2)
         return *s1 - *s2;
   }

   return *s1 - *s2; */
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

function int unusedTID(int start, int end)
{
    int ret = start - 1;
    int tidNum;

    if (start > end) { start ^= end; end ^= start; start ^= end; }  // good ol' XOR swap
    
    while (ret++ != end)
    {
        if (ThingCount(0, ret) == 0)
        {
            return ret;
        }
    }
    
    return -1;
}

function int ThingCounts(int start, int end)
{
    int i, ret = 0;

    if (start > end) { start ^= end; end ^= start; start ^= end; }
    for (i = start; i < end; i++) { ret += ThingCount(0, i); }

    return ret;
}

function void saveCVar(int cvar, int val)
{
    int setStr = StrParam(s:"set ", s:cvar, s:" ", d:val);
    int arcStr = StrParam(s:"archivecvar ", s:cvar);
    ConsoleCommand(setStr); ConsoleCommand(arcStr);
}

function int defaultCVar(int cvar, int defaultVal)
{
    int ret = GetCVar(cvar);
    if (ret == 0) { saveCVar(cvar, defaultVal); return defaultVal; }

    return ret;
}