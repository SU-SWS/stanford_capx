(function ($)
{
    $(document).ready(function() {

        $('.view-id-people >> .views-summary').addClass('btn');
        var allStr = document.URL.indexOf('people/all');
        if (allStr != -1) {
            var subUrl = document.URL.substr(0,allStr+10);
            var newUrl = '<div class="views-summary views-summary-unformatted btn"><a href="'+subUrl+'">All</a></div>';
            $('.view-display-id-glossary > .view-content').prepend(newUrl);
        }

        $("#edit-name").blur(function(event) {
            if ($(this).val() != 0) {
                $("#edit-field-ses-associate-type-tid-1").val("All");
                $("#edit-field-secondary-affiliations-value").val("All");
            }        
        });

        $("#edit-field-ses-associate-type-tid-1").blur(function(event) {
            if ($(this).val() != 'All') {
                $("#edit-name").val('');
            }
        });

        $("#edit-field-secondary-affiliations-value").blur(function(event) {
            if ($(this).val() != 'All') {
                $("#edit-name").val('');
            }
        });

    });


}(jQuery));
