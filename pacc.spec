Name:           pacc
Version:        0.3
Release:        3%{?dist}
Summary:        pacc: a compiler-compiler for PEGs
License:        GPLv3+
URL:            http://paccrat.org/
Source0:        http://static.paccrat.org/release/%{name}-%{version}.tar.bz2

BuildRequires:	bash
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	diffutils
BuildRequires:	elfutils
BuildRequires:	findutils
BuildRequires:	gawk
BuildRequires:	gc
BuildRequires:	gc-devel
BuildRequires:	gcc
BuildRequires:	glibc
BuildRequires:	glibc-common
BuildRequires:	glibc-devel
BuildRequires:	glibc-headers
BuildRequires:	grep
BuildRequires:	kernel-headers
BuildRequires:	libgcc
BuildRequires:	make
BuildRequires:	sed
BuildRequires:	tar

%description
pacc: a compiler-compiler

Pacc reads a description of a language written using PEG (parsing
expression grammar) syntax. It emits C code to parse that language.

PEGs are easy to understand, and pleasant to work with.

%prep
%autosetup


%build
%make_build


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}/
cp pacc %{buildroot}%{_bindir}/
mkdir -p %{buildroot}%{_infodir}/ %{buildroot}%{_mandir}/man1/
cp doc/pacc.info %{buildroot}%{_infodir}/
gzip < doc/pacc.man > %{buildroot}%{_mandir}/man1/pacc.1.gz

%check
make check

%files
%license COPYING
%doc INSTALL README
%{_bindir}/*
%{_infodir}/*
%{_mandir}/man1/*



%changelog
* Thu Jul 21 2016 Toby Goodwin <toby@paccrat.org>
- 
