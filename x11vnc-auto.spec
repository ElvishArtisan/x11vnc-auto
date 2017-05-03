Name:		x11vnc-auto
Version:	0.6.0
Release:	1%{?dist}
Summary:	Automatic VNC login for display:0
License:	GPLv2
Source:		%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Requires:       x11vnc tigervnc-server-minimal


%description
x11vnc-auto is a set of two Systemd services that together provides the
ability to configure an integrated VNC server that will start automatically
at boot time and provide access to display :0.


%prep


%setup -q -n x11vnc-auto


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system
cp x11vnc-auto@.service $RPM_BUILD_ROOT/usr/lib/systemd/system/
cp x11vnc-auto.socket $RPM_BUILD_ROOT/usr/lib/systemd/system/


%clean
rm -rf %{buildroot}


%post
/bin/systemctl daemon-reload


%preun
systemctl stop x11vnc-auto.socket
systemctl disable x11vnc-auto.socket


%postun
/bin/systemctl daemon-reload


%files
%defattr(-,root,root)
/usr/lib/systemd/system/x11vnc-auto@.service
/usr/lib/systemd/system/x11vnc-auto.socket
%doc COPYING
%doc README


%changelog
* Fri Mar 31 2017 Fred Gleason <fredg@paravelsystems.com> - 0.6.0-1
-- Initial packaging.
