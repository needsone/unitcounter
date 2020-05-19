<!DOCTYPE HTML>
<!--
	Prologue by Pixelarity
	html5up.net | @ajlkn
	License: pixelarity.com/license
-->
<html>
	<head>
		<title>UnitCounter</title>
		<meta charset="utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1" />
		<!--[if lte IE 8]><script src="assets/js/ie/html5shiv.js"></script><![endif]-->
		<link rel="stylesheet" href="assets/css/main.css" />
		<!--[if lte IE 8]><link rel="stylesheet" href="assets/css/ie8.css" /><![endif]-->
		<!--[if lte IE 9]><link rel="stylesheet" href="assets/css/ie9.css" /><![endif]-->
	</head>
	<body>
<?php
// if (isset($_GET['submit'])) {
	$action=$_GET['action'];
	$value=$_GET['value'];
	switch ($action) {
    case 'init':
		 $url="http://".$_SERVER['SERVER_ADDR']."/arduino/init/".$value."";
		 $ch = curl_init();
		 curl_setopt($ch, CURLOPT_URL,$url);
		 curl_exec($ch);
		 curl_close($ch);
		break;
    case 'add':
		$url="http://".$_SERVER['SERVER_ADDR']."/arduino/add/".$value."";
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL,$url);
		curl_exec($ch);
		curl_close($ch);
		break;
    case 'rm':
		$url="http://".$_SERVER['SERVER_ADDR']."/arduino/rm/".$value."";
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL,$url);
		curl_exec($ch);
		curl_close($ch);
		break;
	}
// }
?>
		<!-- Header -->
			<div id="header">

				<div class="top">

					<!-- Logo -->
						<div id="logo">
							<span class="image avatar48"><img src="images/avatar.jpg" alt="" /></span>
							<h1 id="title">Office box</h1>
							<p>Gérer les cartes</p>
						</div>

					<!-- Nav -->
						<nav id="nav">
							<ul>
								<li><a href="#top" id="top-link" class="skel-layers-ignoreHref"><span class="icon fa-home">Home</span></a></li>
								<li><a href="#init" id="about-link" class="skel-layers-ignoreHref"><span class="icon fa-user">Init card</span></a></li>
								<li><a href="#add" id="portfolio-link" class="skel-layers-ignoreHref"><span class="icon fa-credit-card">add unit</span></a></li>
								<li><a href="#rm" id="contact-link" class="skel-layers-ignoreHref"><span class="icon fa-credit-card">remove unit</span></a></li>
								<li><a href="#rm" id="contact-link" class="skel-layers-ignoreHref"><span class="icon fa-credit-card">Network setting</span></a></li>
							</ul>
						</nav>

				</div>
			</div>
		<!-- Main -->
			<div id="main">
				<!-- Intro -->
					<section id="top" class="one dark cover">
						<div class="container">
							<header>
								<h2 class="alt">Interface pour UnitCounter<br />
								Créer vos cartes pour chaque utilisateur</h2>
								<p>Ajouter et enlever des unités pour vos cartes<br />
								Voir toutes les transactions.</p>
							</header>
							<footer>
								<a href="#init" class="button scrolly">Déposez une carte sur le boitier</a>
							</footer>
						</div>
					</section>

				<!-- Portfolio -->
					<section id="init" class="two">
						<div class="container">
							<header>
								<h2>Effacer et ajouter un client pour la carte</h2>
							</header>
							<form method="get" action="./index.php">
								<div class="row">
									<div class="12u$"><input type="text" name="value" placeholder="Nom du client" /></div>
									<div class="12u$"><input type="hidden" name="action" value="init" /></div>
									<div class="12u$">
										<input type="submit" value="Initaliser la carte" />
									</div>
								</div>
							</form>
					</div>
					<section id="add" class="two">
						<div class="container">
							<header>
								<h2>Ajouter des unités.</h2>
							</header>
							<form method="get" action="./index.php">
								<div class="row">
									<div class="12u$"><input type="text" name="value" placeholder="Nombre de minutes" /></div>
									<div class="12u$"><input type="hidden" name="action" value="add" /></div>
									<div class="12u$">
										<input type="submit" value="Ajouter des unités" />
									</div>
								</div>
							</form>
						</div>
					</section>
					<section id="rm" class="four">
						<div class="container">
							<header>
								<h2>Enlever des unités.</h2>
							</header>
							<form method="get" action="./index.php">
								<div class="row">
									<div class="12u$"><input type="text" name="value" placeholder="Nombre de minutes" /></div>
									<div class="12u$"><input type="hidden" name="action" value="rm" /></div>
									<div class="12u$">
										<input type="submit" value="Ajouter des unités" />
									</div>
								</div>
							</form>
						</div>
					</section>
			</div>
		</div>

		<!-- Footer -->
			<div id="footer">
				<!-- Copyright -->
					<ul class="copyright">
						<li>&copy; UnitCounter. All rights reserved.</li>
					</ul>

			</div>
		<!-- Scripts -->
			<script src="assets/js/jquery.min.js"></script>
			<script src="assets/js/jquery.scrolly.min.js"></script>
			<script src="assets/js/jquery.scrollzer.min.js"></script>
			<script src="assets/js/skel.min.js"></script>
			<script src="assets/js/util.js"></script>
			<!--[if lte IE 8]><script src="assets/js/ie/respond.min.js"></script><![endif]-->
			<script src="assets/js/main.js"></script>

	</body>
</html>
