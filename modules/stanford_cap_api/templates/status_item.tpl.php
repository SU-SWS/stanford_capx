<?php
/**
 * @file
 * Status item template.
 */

/**
 * Variables:
 *
 * @var bool $status
 * @var string $message
 * @var string $additional
 */

$status_class = $status ? 'cap_status_good' : 'cap_status_bad';
?>
<div class="cap_info_wrapper">
  <div class="form-wrapper" id="edit-status-wrapper">
    <div class="cap_checkbox <?php print $status_class; ?>"><span></span></div>
    <?php print $message;
      if (isset($additional)) :
    ?>
      <br / >
    <em class="cap_additional"><?php print $additional; ?></em>
    <?php endif; ?>
  </div>
  <div class="clearfix"></div>
</div>
