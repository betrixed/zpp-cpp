<?php

class Asserts {

	protected ?string $expect_throw = null;
        
	protected function setUp(): void
	{

	}

	protected function tearDown(): void
	{

	}

	public function run(string $method)
	{
        $this->expect_throw = null;
		$this->setUp();
        try {
            $this->$method();
        }
        catch(Throwable $x) 
        {
            $classexpect = $this->expect_throw;
            if (!is_string($classexpect))
            {
                throw new Exception("Unexpected Throwable with msg: " . $x->getMessage());;
            } 
            else {
                $xclass = get_class($x);
                if ($classexpect === $xclass) {
                    echo "Exception class match of $xclass\n";
                }
                else {
                    throw new Exception("Unexpected $xclass: " . $x->getMessage());
                }
            }
            $classexpect = null; // clear expectation
        }
		$this->tearDown();
	}

	public function assertCount(int $match, mixed $m)
	{
		if (count($m) !== $match)
		{
			throw new Exception("Count is not $match");
		}
		echo ".";
	}

	public function assertStringStartsWith(string $needle, string $haystack)
	{
		if (!str_starts_with($haystack, $needle))
		{
			throw new Exception("String does not start with $needle" );
		}
		echo ".";
	}

	public function assertIsArray(mixed $value)
	{
		if (!is_array($value))
		{
			throw new Exception("Value is not array - " . gettype($value));
		}
		echo ".";
	}

	public function assertNotNull(mixed $value)
	{
		if (is_null($value))
		{
			throw new Exception("Value is Null");
		}
		echo ".";
	}

	public function assertIsClass(mixed $value, string $classname)
	{
		$this->assertIsObject($value);
		$this->assertEquals(get_class($value), $classname);
		echo ".";

	}
	public function assertIsObject(mixed $value)
	{
		if (!is_object($value))
		{
			throw new Exception("Value is not type object - " . gettype($value));
		}
		echo ".";
	}
        
        public function assertIsString(mixed $value)
        {
 		if (!is_string($value))
		{
			throw new Exception("Value is not type string - " . gettype($value));
		}
		echo ".";           
        }
        
    public function assertNotEquals(mixed $v1, mixed $v2)
    {
    	if (gettype($v1) === gettype($v2))
    	{
    		if ($v1 === $v2)
    		{
    			throw new Exception("Equal values of type " . gettype($v1));
    		}
    	}
    	echo ".";   
    }
	public function assertEquals(mixed $v1, mixed $v2)
	{
		if (gettype($v1) !== gettype($v2))
		{
			throw new Exception("Values not same type");
		}
		if (is_object($v1))
		{
			if (get_class($v1) !== get_class($v2))
			{
				throw new Exception("Values not same class");
			}
		}
		if (is_string($v1) || is_int($v1))
		{
			if ($v1 !== $v2)
			{
				throw new Exception("$v2 !== $v1");
			}
		}
		echo ".";

	}
	public function assertFileExists(string $path)
	{
		if (!file_exists($path))
		{
			throw new Exception("File $path does not exist");
		}
		echo ".";
	}

	public function assertDirectoryExists(string $path)
	{
		if (!is_dir($path))
		{
			throw new Exception("$path is not a directory");
		}
		echo ".";
	}
	public function assertEmpty(mixed $value)
	{
		if (!empty($value))
		{
			throw new Exception(print_r($value,true) . " is Not Empty");
		}
		echo ".";
	}
	public function assertNotEmpty(mixed $value, string $msg = "Value")
	{
		if (empty($value))
		{
			throw new Exception($msg . " is Empty");
		}
		echo ".";
	}
        
        public function expectException(string $exception_class)
        {
            $this->expect_throw = $exception_class;
        }
        
	public function assertTrue(mixed $value)
	{
		if ($value !== true)
		{
			throw new Exception("Value is not true");
		}
		echo ".";
	}
	public function assertFalse(mixed $value)
	{
		if ($value !== false)
		{
			throw new Exception("Value is not false");
		}
		echo ".";
	}
}