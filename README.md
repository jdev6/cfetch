# cfetch

intended replacedment for screenfetch/neofetch. in c so a bunch faster

submit prs for new ascii images AND distro ids (found in /etc/lsb-release)

# building

just run `make`. you will need dpendencies that are below and it leaves exectable 'cfetch'

however if you have a error or something doesnt show.. use `make debug`. it will leave 'cfetch.debug' executable and it prints infomation whebn something fails

and `make install` to install in /usr/bin.. probably needs sudo

# dependencies

Xlib (sorry for wayland., to get screen resolution),and  libcpuid for CPU  info and pcilib (probably allready installed.) for gpu info
