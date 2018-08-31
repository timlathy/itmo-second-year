# Lab1

This is [my first experience using PHP](https://timlathy.github.io/study-notes/2018/08/02/my-experience-with-php.html).

Although the purpose of the system was quite simple — check whether a given point is inside a polygon —
it turned out to be a bit of a Rube Goldberg machine. I'll see if I can strip out the unnecessary parts later.

## Development

```
# Fetch dependencies:
composer install

# Run tests:
composer test

# Compile client-side ReasonML scripts to JS upon change:
( cd client; npm run watch )
```

## Deployment

I wish the following was a joke, but alas, it is not.

```
# Prepare the assets:
( cd client; npm run buildprod )

# Archive the application:
tar cf lab1.tar client/dist src vendor

# Copy the archive over to the target server:
scp lab1.tar prod:/target/dir

# Unpack it:
tar xf lab1.tar

# Adjust asset paths:
sed -i 's|src/Main.bs.js|dist/app.js|g' src/templates/_head.html.php
```
