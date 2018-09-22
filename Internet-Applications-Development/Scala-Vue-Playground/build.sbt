import Dependencies._

enablePlugins(JettyPlugin)

lazy val root = (project in file(".")).
  settings(
    name := "playground",
    inThisBuild(List(
      organization := "com.example",
      scalaVersion := "2.12.6",
      version      := "0.1.0-SNAPSHOT"
    )),
    javacOptions ++= Seq("-source", "1.8", "-target", "1.8"),
    libraryDependencies += scalaTest % Test,
    libraryDependencies += "javax.servlet" % "javax.servlet-api" % "4.0.1" % "provided",
    libraryDependencies += "org.scala-lang.modules" %% "scala-xml" % "1.1.0"
  )

