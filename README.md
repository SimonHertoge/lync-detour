# Lync detour

PoC to bypass GPOs defined in HKCU using MS Detours injection.

This tool adds the Appear Offline status to Skype for Business / Lync, which otherwise has to be set through a company GPO.

## Build

Install the [Detours package](https://www.nuget.org/packages/Detours/) and compile the whole solution in VS19, or use nmake inside the VS19 Command Prompt. 