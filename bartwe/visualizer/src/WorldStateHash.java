import java.util.Arrays;

public class WorldStateHash {
    private byte[] digest;
    private int hash;

    public WorldStateHash(byte[] digest) {
        this.digest = digest;
        hash = Arrays.hashCode(digest);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        WorldStateHash that = (WorldStateHash) o;
        if (hash != that.hash)
            return false;
        return Arrays.equals(digest, that.digest);
    }

    @Override
    public int hashCode() {
        return hash;
    }
}
