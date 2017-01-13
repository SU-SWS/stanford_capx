<?php
/**
 * 
 */
class Foo {
    public function bar($param)  {
        if ($param === 42) {
            exit(23);
        }
         if ($param === 42) {
            eval('$param = 23;');
        }
    }
}

lass Foo2 {
    /**
     * @var \foo\bar\X
     */
    private $x = null;

    /**
     * @var \foo\bar\Y
     */
    private $y = null;

    /**
     * @var \foo\bar\Z
     */
    private $z = null;

    public function setFoo(\Foo2 $foo) {}
    public function setBar(\Bar $bar) {}
    public function setBaz(\Baz $baz) {}

    /**
     * @return \SplObjectStorage
     * @throws \OutOfRangeException
     * @throws \InvalidArgumentException
     * @throws \ErrorException
     */
    public function process(\Iterator $it) {}

}
