// here is some simple code to hide the variable mapping when the 
// user changes the selected bundle. this should eventually be
// replaced by some fancy-pants AHAH or AJAX or whatever they're 
// calling it these days to update the form for the selected bundle
(function ($)
{
    $(document).ready(function() {

        $('#edit-cap-lite-container1').hide();
        $('#edit-ses-cap-lite-bundle').change(function() {
            if ($(this).val() == $('#ses-cap-lite-originalb').val()) {
                $('#edit-cap-lite-container1').hide();
                $('#edit-cap-lite-container2').show();
                $('#capfields-items-table').show();
            } else {
                $('#edit-cap-lite-container2').hide();
                $('#edit-cap-lite-container1').show();
                $('#capfields-items-table').hide();
            }
        });

    });

}(jQuery));
