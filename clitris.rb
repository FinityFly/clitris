class Clitris < Formula
  desc "Command Line Interface Tetris"
  homepage "https://github.com/FinityFly/clitris"
  url "https://github.com/FinityFly/clitris/archive/refs/tags/v0.0.1.tar.gz"
  sha256 "43f7ac337d0493ce874451e123b020f8d649a4bd747eb6f44f93ec1b55a618cb"
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
