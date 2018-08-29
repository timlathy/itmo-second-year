<?php

class LineException extends Exception {
  private $line_index = 0;

  public function __construct(string $message, int $line_index) {
    $this->line_index = $line_index;

    parent::__construct($message, 0, null);
  }

  public function js_error_object(): array {
    return ["line" => $this->line_index, "message" => $this->message];
  }
}
