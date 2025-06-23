class Clitris < Formula
  desc "Command Line Interface Tetris"
  homepage "https://github.com/FinityFly/clitris"
  url "https://github.com/FinityFly/clitris/archive/refs/tags/v0.0.2.tar.gz"
  sha256 "0baca359e3fc106c4277d6c9a6c15549519c87151f291547f49dd9e7e5db5ad1"
  license "MIT"

  uses_from_macos "ncurses"

  def install
    system "make"
    bin.install "tetris"
  end

  test do
    assert_match "usage", shell_output("#{bin}/tetris --help 2>&1", 1)
  end
end
