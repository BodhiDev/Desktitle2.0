[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/paypalme/rbtylee)

# Desktitle2.0

This module is a [Bodhi Linux](https://www.bodhilinux.com/) fork and improvement of the abandoned and no longer functional module [desktitle](https://github.com/tamentis/desktitle).

The module displays a virtual desktops name and allows one to quickly rename the virtual desktop by double clicking the name. Text color can be changed via the module's settings. Thanks to  Štefan Uram for seeing the necessity of this option and implementing it. Any further customizations of this modules apperance require modifying the modules theme (edj file).
<div align="center">
  <img src="https://i.imgur.com/Cj1Snsc.png">
</div>

# Rationale

At first glance one may wonder why would one care what name is assigned to a virtual desktop? The module is aimed at users that users that like to organize their workflow by keeping all windows deciated a certain task on one desktop and switch back and forth between tasks by changing the desktop displayed. This module provides an easy way to see what desktop one is currently at. Since the pager module does not display a virtual desktops name but the virtual desktop settings allow one to change the names, this module may make your workflow easier. This is especially true if one prefers keyboard shortcuts to change desktops and or does not even use the pager module or else it is covered up by windows. Desktitle is a bit of fluff designed to make life a little easier for such users.

# Dependencies

* The usual build tools, autopoint libtool intltool pkg-config  autotools-dev
* [EFL](https://www.enlightenment.org/download)
* [Moksha](https://github.com/JeffHoogland/moksha)

# Installation

It is recommended Bodhi 5.0 users install from Bodhi's repo:

```ShellSession
sudo apt update
sudo apt install moksha-module-desktitle
```
> Note: It will be added to Bodhi's repos soon

Other users need to compile the code:

First install all the needed dependencies. Note this includes not only EFL but the EFL header files. If you have compiled and installed EFL, and Moksha from source code this should be no problem. 

Then the usual:

```ShellSession
./autogen.sh
make
sudo make install
```

# Pure enlightenment

It is our hope to create branches for e17 and enlightenment. Then to install in these environments clone the repo and switch to the needed branch and compile as usual.

# Reporting bugs

Please use the GitHub issue tracker for any bugs or feature suggestions.

# Contributing

Help is always Welcome, as with all Open Source Projects the more people that help the better it gets!

Please submit patches to the code or documentation as GitHub pull requests!

Contributions must be licensed under this project's copyright (see COPYING).

# Help wanted

Nothing in this module has been translated. The needed PO files have been created but we are requesting users of this modules contribute the needed localizations.

Developers may wish to examine our todo file and help implement future features.

Thanks in advance.

# Support This Project

This module is part of our current project to restore to functioning all broken e17 modues we know about. These modules can be broken by enlightenment code changes or EFL API changes. 

Donations to [Bodhi Linux](https://www.bodhilinux.com/donate/) would be greatly appreciated and keep our distro moving along. But if you like the work we do for Bodhi and wish to see more of it, we'd be happy about a donation. You can either donate via [PayPall](https://www.paypal.com/paypalme/rbtylee) or [Liberapay](https://liberapay.com/ylee/). 

# License

This software is released under the same License used in alot of the other Enlightenment projects. It is a custom license but fully Open Source. Please see the included [COPYING](https://github.com/rbtylee/launcher-spellchecker/blob/master/COPYING) file and for a less legalese explanation [COPYING-PLAIN](https://github.com/rbtylee/launcher-spellchecker/blob/master/COPYING-PLAIN).

Simply put, this software is free to use, modify and redistribute as you see fit. I do ask that you keep the copyright notice the same in any modifications.

The debian files are  released the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl.html) as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


# Credits

Full credit for the original code of this module go to:
* _*Bertrand Janin*_

Bodhi specific modifications, improvements and code modernization go to :
* _*Robert Wiley*_
* _*Štefan Uram*_
