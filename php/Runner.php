<?php

class Runner {

	public function runObject(object $obj)
	{
		$m = get_class_methods($obj);

		foreach($m as $name)
		{
			if (str_starts_with($name, "test"))
			{
				echo "\nTest $name [";
				$obj->run($name);
			}
		}
		echo "\nAll Done\n";
	}

}