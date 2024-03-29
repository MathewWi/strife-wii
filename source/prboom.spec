%define doomname prboom

Name: %{doomname}
Summary: Doom - classic 3D shoot-em-up game
Version: 2.2.3
Release: 1
Source: ftp://download.sourceforge.net/prboom/prboom-2.2.3.tar.gz
URL: http://prboom.sourceforge.net/
Group: Amusements/Games
BuildRoot: /var/tmp/%{doomname}-buildroot
Copyright: GPL
Packager: Colin Phipps <cphipps@doomworld.com>
Prefix: %{_prefix}
Autoreq: 0
Requires: libSDL-1.2.so.0, libSDL_mixer-1.2.so.0, libSDL_net-1.2.so.0, libm.so.6, libc.so.6

%description
Doom is the classic 3D shoot-em-up game. It must have been one of the best
selling games ever; it totally outclassed any  3D world games that preceded
it, with amazing speed, flexibility, and outstanding gameplay. The specs to
the game were released, and thousands of extra levels were written by fans of
the game; even today new levels are written for Doom faster then any one person
could play them. 

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
./configure --disable-cpu-opt \
 	--prefix=/usr \
	--exec-prefix=/usr \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	--sysconfdir=/etc \
	--datadir=/usr/share \
	--includedir=/usr/include \
	--libdir=/usr/lib \
	--libexecdir=/usr/lib \
	--localstatedir=/var/lib \
	--sharedstatedir=/usr/com \
	--mandir=/usr/share/man \
	--infodir=/usr/share/info
make

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc %{_mandir}/man5/*
%doc %{_mandir}/man6/*
%doc NEWS
%doc AUTHORS
%doc README
%doc COPYING
%doc doc/README.compat
%doc doc/README.demos
%doc doc/MBF.txt
%doc doc/MBFFAQ.txt
%doc doc/boom.txt
/usr/games/*
%{_datadir}/games/doom/prboom.wad

