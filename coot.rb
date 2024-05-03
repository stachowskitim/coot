class Coot < Formula
  include Language::Python::Virtualenv

  desc "Crystallographic Object-Oriented Toolkit"
  homepage "https://www2.mrc-lmb.cam.ac.uk/personal/pemsley/coot/"
  #url "https://www2.mrc-lmb.cam.ac.uk/personal/pemsley/coot/source/releases/coot-1.1.07.tar.gz"
  sha256 "d1b01cc58f970cec8e80200344439c0aed9843702578ea02f1cc7d4395f9e30b"
  license any_of: ["GPL-3.0-only", "LGPL-3.0-only", "GPL-2.0-or-later"]

  #bottle do
  #  root_url "http://vivace.bi.a.u-tokyo.ac.jp"
  #  sha256 cellar: :any,                 arm64_sonoma:   "e71f1f74f0d813b71c6d844b25f2872e64b8dbce552291428d1ce23db0f0aa55"
  #  sha256 cellar: :any,                 sonoma:         "7acb660face4618663022a91bde53bb73a2184f0d1e46b5093c16319e5d58d94"
  #end

  head do
    url "https://github.com/stachowskitim/coot.git", branch: "main"
    depends_on "autoconf" => :build
    depends_on "automake" => :build
    depends_on "libtool" => :build
    depends_on "swig" => :build
  end

  depends_on "glm" => :build
  depends_on "pkg-config" => :build
  depends_on "adwaita-icon-theme" # display icons
  depends_on "boost"
  depends_on "boost-python3"
  depends_on "brewsci/bio/clipper4coot"
  depends_on "brewsci/bio/gemmi"
  depends_on "brewsci/bio/libccp4"
  depends_on "brewsci/bio/mmdb2"
  depends_on "brewsci/bio/raster3d"
  depends_on "brewsci/bio/ssm"
  depends_on "dwarfutils"
  depends_on "glfw"
  depends_on "glib"
  depends_on "gmp"
  depends_on "gsl"
  depends_on "gtk4"
  depends_on "libepoxy"
  depends_on "numpy"
  depends_on "py3cairo"
  depends_on "pygobject3"
  depends_on "python@3.12"
  depends_on "rdkit"
  depends_on "sqlite"

  uses_from_macos "bzip2"
  uses_from_macos "curl"

  resource "certifi" do
    url "https://files.pythonhosted.org/packages/98/98/c2ff18671db109c9f10ed27f5ef610ae05b73bd876664139cf95bd1429aa/certifi-2023.7.22.tar.gz"
    sha256 "539cc1d13202e33ca466e88b2807e29f4c13049d6d87031a3c110744495cb082"
  end

  resource "charset-normalizer" do
    url "https://files.pythonhosted.org/packages/cf/ac/e89b2f2f75f51e9859979b56d2ec162f7f893221975d244d8d5277aa9489/charset-normalizer-3.3.0.tar.gz"
    sha256 "63563193aec44bce707e0c5ca64ff69fa72ed7cf34ce6e11d5127555756fd2f6"
  end

  resource "idna" do
    url "https://files.pythonhosted.org/packages/8b/e1/43beb3d38dba6cb420cefa297822eac205a277ab43e5ba5d5c46faf96438/idna-3.4.tar.gz"
    sha256 "814f528e8dead7d329833b91c5faa87d60bf71824cd12a7530b5526063d02cb4"
  end

  resource "monomers" do
    url "https://www2.mrc-lmb.cam.ac.uk/personal/pemsley/coot/dependencies/refmac-monomer-library.tar.gz"
    sha256 "03562eec612103a48bd114cfe0d171943e88f94b84610d16d542cda138e5f36b"
  end

  resource "reference-structures" do
    url "https://www2.mrc-lmb.cam.ac.uk/personal/pemsley/coot/dependencies/reference-structures.tar.gz"
    sha256 "44db38506f0f90c097d4855ad81a82a36b49cd1e3ffe7d6ee4728b15109e281a"
  end

  resource "requests" do
    url "https://files.pythonhosted.org/packages/9d/be/10918a2eac4ae9f02f6cfe6414b7a155ccd8f7f9d4380d62fd5b955065c3/requests-2.31.0.tar.gz"
    sha256 "942c5a758f98d790eaed1a29cb6eefc7ffb0d1cf7af05c3d2791656dbd6ad1e1"
  end

  resource "pandas" do
    url "https://files.pythonhosted.org/packages/03/d2/6fb05f20ee1b3961c7b283c1f8bafc6de752155d075c5db61c173de0de62/pandas-2.2.0.tar.gz"
    sha256 "30b83f7c3eb217fb4d1b494a57a2fda5444f17834f5df2de6b2ffff68dc3c8e2"
  end

  resource "numpy" do
    url "https://files.pythonhosted.org/packages/65/6e/09db70a523a96d25e115e71cc56a6f9031e7b8cd166c1ac8438307c14058/numpy-1.26.4.tar.gz"
    sha256 "2a02aba9ed12e4ac4eb3ea9421c420301a0c6460d9830d74a9df87efa4912010"
  end

  resource "matplotlib" do
    url "https://files.pythonhosted.org/packages/fb/ab/38a0e94cb01dacb50f06957c2bed1c83b8f9dac6618988a37b2487862944/matplotlib-3.8.2.tar.gz"
    sha256 "01a978b871b881ee76017152f1f1a0cbf6bd5f7b8ff8c96df0df1bd57d8755a1"
  end

  resource "scipy" do
    url "https://files.pythonhosted.org/packages/30/85/cdbf2c3c460fe5aae812917866392068a88d02f07de0fe31ce738734c477/scipy-1.12.0.tar.gz"
    sha256 "4bf5abab8a36d20193c698b0f1fc282c1d083c94723902c447e5d2f1780936a3"
  end

  resource "matplotlib-venn" do
    url "https://files.pythonhosted.org/packages/61/fe/5a36da0d3dde780e2839f20934f7e0e8c8aea682a3de93bf6049e10d147f/matplotlib-venn-0.11.10.tar.gz"
    sha256 "90d0cfb279c5dbbdf7dfd722c0e2515a37f535e949bcad174483bc777e372e65"
  end

  resource "urllib3" do
    url "https://files.pythonhosted.org/packages/af/47/b215df9f71b4fdba1025fc05a77db2ad243fa0926755a52c5e71659f4e3c/urllib3-2.0.7.tar.gz"
    sha256 "c97dfde1f7bd43a71c8d2a58e369e9b2bf692d1334ea9f9cae55add7d0dd0f84"
  end

  def python3
    which("python3.12")
  end

  def install
    ENV.cxx11
    ENV.libcxx
    if build.head?
      # libtool -> glibtool for macOS
      inreplace "autogen.sh", "libtool", "glibtool"
      system "./autogen.sh"
    end
    if OS.mac?
      inreplace "./configure", "$wl-flat_namespace", ""
      inreplace "./configure", "$wl-undefined ${wl}suppress", "-undefined dynamic_lookup"
    end

    # Get Python location
    xy = Language::Python.major_minor_version python3
    # Install Python dependencies
    venv = virtualenv_create(libexec, python3)
    %w[certifi charset-normalizer idna requests urllib3].each do |r|
      venv.pip_install resource(r)
    end
    (lib/"python#{xy}/site-packages/homebrew-coot.pth").write "#{libexec/"lib/python#{xy}/site-packages"}\n"
    ENV.prepend_path "PYTHONPATH", libexec/"lib/python#{xy}/site-packages"

    # Set Boost, RDKit, and FFTW2 root
    boost_prefix = Formula["boost"].opt_prefix
    boost_python_lib = "boost_python312-mt"
    rdkit_prefix = Formula["rdkit"].opt_prefix
    fftw2_prefix = Formula["clipper4coot"].opt_prefix/"fftw2"

    args = %W[
      --prefix=#{prefix}
      --with-enhanced-ligand-tools
      --with-boost=#{boost_prefix}
      --with-boost-libdir=#{boost_prefix}/lib
      --with-rdkit-prefix=#{rdkit_prefix}
      --with-fftw-prefix=#{fftw2_prefix}
      --with-backward
      --with-libdw
      BOOST_PYTHON_LIB=#{boost_python_lib}
    ]

    ENV.append_to_cflags "-fPIC" if OS.linux?
    system "./configure", *args
    system "make"
    # fix executable path
    inreplace "src/Coot", "progdir=\"$thisdir/.libs\"", "progdir=\"$thisdir/../libexec\""
    ENV.deparallelize { system "make", "install" }

    # install reference data
    # install data, #{pkgshare} is /path/to/share/coot
    (pkgshare/"reference-structures").install resource("reference-structures")
    (pkgshare/"lib/data/monomers").install resource("monomers")
  end

  # test block is not tested now.
  test do
    assert_match "Usage: coot", shell_output("#{bin}/coot --help 2>&1")
  end
end
  # test block is not tested now.
  test do
    assert_match "Usage: coot", shell_output("#{bin}/coot --help 2>&1")
  end
end
