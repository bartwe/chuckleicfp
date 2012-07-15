import java.util.Arrays;

public class WorldStateHash {
    private final byte[] digest;
    private final int hash;

    public WorldStateHash(byte[] digest) {
        this.digest = digest;
        hash = Arrays.hashCode(digest);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        WorldStateHash that = (WorldStateHash) o;
        return (hash == that.hash) && Arrays.equals(digest, that.digest);
    }

    @Override
    public int hashCode() {
        return hash;
    }
}
