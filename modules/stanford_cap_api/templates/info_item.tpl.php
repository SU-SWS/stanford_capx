<?php
/**
 * @file
 * Info item template.
 */

/**
 * Variables:
 * @var string $info
 * @var string $message
 * @var string $additional
 */
?>
<div class="cap_info_wrapper">
  <div class="form-wrapper" id="edit-status-wrapper">
    <div class="cap_checkbox cap_info"><span><?php print $info; ?></span></div>
    <?php print $message;
      if (isset($additional)) :
    ?>
      <br / >
      <em class="cap_additional"><?php print $additional; ?></em>
    <?php endif; ?>
  </div>
  <div class="clearfix"></div>
</div>
