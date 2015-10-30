<!DOCTYPE html>
<html lang="en">
    <head>
        %include header
    </head>
    <body>
        <!-- Include each module that makes up each page -->
        %include navbar
        
        <div class="container-fluid">
            <div class="row" id="header_image">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <img src="static/header_logo.png" class="img-responsive" alt="RN1723 Development Kit Server">
                </div>
            </div>
        </div>
        <!-- Load the rest of the page contents based on status of cookie -->
        <div class="container-fluid">
            <div class="row">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <p class="panel-title">Release Notes</p>
                        </div>
                        <div class="panel-body">
                            <h4>Version 1.0</h4>
                            <ul>
                                <li>This is the initial release of the Server for the RN1723 Development Kit</li>
                            </ul>
                            <h4>Version 4.0</h4>
                            <ul>
                                <li>This is the initial release of the Server for the RN1723 Development Kit</li>
                            </ul>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Include the javascript for the page -->
        <script src="static/jquery-1.11.1.min.js"></script>
        <script src="static/bootstrap.min.js"></script>
    </body>

    <footer>
        %include footer
    </footer>

</html>
