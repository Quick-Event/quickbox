Summary: Open Source QT Database administration tool
Name:    qsqlmon
Source0: %{name}-%{version}.tar.bz2
Version: 1.3.5-git.1330959505
Release: 0%{?dist}
License: GPLv3
Group:   Productivity/Databases/Tools
URL:     http://qsqlmon.sourceforge.net
BuildRoot: /var/tmp/%{name}-root

%if 0%{?suse_version}
BuildRequires:  libqt4-devel libQtWebKit-devel mysql-devel sqlite3-devel postgresql-devel gcc-c++ make  update-desktop-files
%endif
%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version}
BuildRequires:  qt4-devel qt-webkit-devel mysql-devel sqlite-devel postgresql-devel gcc-c++
%endif

%description
Qt4 Gui tool for SQL server administration. Currently supported servers are PostgreSQL, MySQL, Sqlite and FireBird

%prep
%setup -q

%build
%if 0%{?centos_version} || 0%{?rhel_version}
export PATH=/usr/lib64/qt4/bin:/usr/lib/qt4/bin:$PATH
%endif

%if 0%{?fedora}
CFLAGS="%{optflags}" CXXFLAGS="%{optflags}" \
qmake-qt4 PREFIX=%{_prefix} NOSTRIP=1 NORPATH=1
%else
CFLAGS="%{optflags}" CXXFLAGS="%{optflags}" \
qmake PREFIX=%{_prefix} NOSTRIP=1 NORPATH=1
%endif
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/%{_libdir}/qt4/plugins/
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/32x32/apps
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/48x48/apps
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/96x96/apps

cp _build/bin/{qsqlmon,repedit}  -r $RPM_BUILD_ROOT/usr/bin/
cp _build/lib/*                  -r $RPM_BUILD_ROOT/%{_libdir}/
rm	$RPM_BUILD_ROOT/%{_libdir}/*.so
cp _build/bin/codecs             -r $RPM_BUILD_ROOT/%{_libdir}/qt4/plugins/

cd qsqlmon
sed -i 's|Icon.*qsqlmon*.*|Icon:\ qsqlmon.png|' distro/others/qsqlmon.desktop
cp distro/others/qsqlmon.desktop			$RPM_BUILD_ROOT/usr/share/applications/qsqlmon.desktop
cp distro/icons/qsqlmon32x32.png		$RPM_BUILD_ROOT/usr/share/icons/hicolor/32x32/apps/qsqlmon.png
cp distro/icons/qsqlmon48x48.png		$RPM_BUILD_ROOT/usr/share/icons/hicolor/48x48/apps/qsqlmon.png
cp distro/icons/qsqlmon96x96.png		$RPM_BUILD_ROOT/usr/share/icons/hicolor/96x96/apps/qsqlmon.png
%if 0%{?suse_version}
%suse_update_desktop_file -i -r -G "Database GUI Tool" %{name} "Qt;Development;Database;GUIDesigner;"
%endif

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
/usr/bin/*
%{_libdir}/*.so.*
%dir %{_libdir}/qt4
%dir %{_libdir}/qt4/plugins
%dir %{_libdir}/qt4/plugins/codecs
%{_libdir}/qt4/plugins/codecs/*.so
/usr/share/applications/qsqlmon.desktop
/usr/share/icons/hicolor
%changelog
* Sun Mar 4 2012 Michal Hrusecky <mhrusecky@suse.cz>
- Various cleanup
* Sat May 8 2010 Radu Fiser <radu.fiser@gmail.com>
- File modified after discussions with Frantisek Vacek
* Wed Apr 14 2010 Radu Fiser <radu.fiser@gmail.com>
- Created the file

